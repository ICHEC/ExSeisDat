/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include <functional>
#include "data/datampiio.hh"
#include "anc/piol.hh"
#include "anc/cmpi.hh"
#include "share/smpi.hh"
namespace PIOL { namespace Data {
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////       Non-Class       ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
static MPI_File mopen(ExSeisPIOL & piol, MPI_Comm comm, const MPIIOOpt & opt, const std::string name)
{
    MPI_File file = MPI_FILE_NULL;
    int err = MPI_File_open(comm, name.data(), opt.mode, opt.info, &file);
    //int err = MPI_File_open(comm, name.c_str(), opt.mode, opt.info, &file);

    printErr(piol, name, Log::Layer::Data, err, nullptr, "MPI_File_open failure");

    //I assume this check condition is unnecessary but the spec does not explicitly say what state
    //file is in when there is an error.
    return (err != MPI_SUCCESS ? MPI_FILE_NULL : file);
}

/*! \brief This templated function pointer type allows us to refer to MPI functions more compactly.
 */
template <typename U>
using MFp = std::function<int(MPI_File, MPI_Offset, void *, int, MPI_Datatype, U *)>;

/*! \brief The MPI-IO inline template function for dealing with
 *  integer limits for reading and writing in MPI-IO.
 *  \tparam T The type of the array being read to or from.
 *  \tparam U The type of the last argument of MPI_File_...
 *  \param[in] fn Function pointer to the MPI function.
 *  \param[in] file The MPI File
 *  \param[in] offset The offset from the current shared ptr
 *  \param[in] sz The number of elements to read or write
 *  \param[in] arg The last argument of the MPI_File_... function
 *  \param[in] max The maximum size to read or write at once
 *  \param[out] d The array to read into or from
 *  \return Returns the MPI error code. MPI_SUCCESS for success,
 *  MPI_ERR_IN_STATUS means the status structure should be checked.
 *
 * This function does not currently take into account collective MPI calls
 * which would have issues with the number of operations being the same for each process
 */
template <typename T, typename U = MPI_Status> inline
int io(const MFp<U> fn, const MPI_File & file, csize_t offset, csize_t sz, U & arg, size_t max, T * d, csize_t bsz = 1U, csize_t osz = 1U)
{
    int err = MPI_SUCCESS;

    max /= osz;
    csize_t q = sz / max;
    csize_t r = sz % max;

    for (size_t i = 0U; i < q && err == MPI_SUCCESS; i++)
        err = fn(file, MPI_Offset(offset + osz*i*max), &d[bsz*i*max], max, MPIType<T>(), &arg);

    return (err == MPI_SUCCESS ? fn(file, MPI_Offset(offset + osz*q*max), &d[bsz*q*max], r, MPIType<T>(), &arg) : err);
}

/*! \brief This function exists to hide the const from the MPI_File_write_at function signature
 *  \param[in] f The MPI file handle
 *  \param[in] o The offset in bytes from the current internal shared pointer
 *  \param[in] d The array to read data output from
 *  \param[in] s The amount of data to write to disk in terms of datatypes
 *  \param[in] da The MPI datatype
 *  \param[in] st The MPI status structure
 *  \return Returns the associated MPI error code.
 */
int mpiio_write_at(MPI_File f, MPI_Offset o, void * d, int s, MPI_Datatype da, MPI_Status * st)
{
    return MPI_File_write_at(f, o, d, s, da, st);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    Class functions    ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////      Constructor & Destructor      ///////////////////////////////
MPIIOOpt::MPIIOOpt(void)
{
    mode = MPI_MODE_RDONLY | MPI_MODE_UNIQUE_OPEN;
    info = MPI_INFO_NULL;
    fcomm = MPI_COMM_SELF;
    maxSize = getLim<int32_t>();
}

MPIIO::MPIIO(Piol piol_, const std::string name_, const MPIIOOpt & opt) : PIOL::Data::Interface(piol_, name_)
{
    maxSize = opt.maxSize;
    info = opt.info;

    MPI_Aint lb, esz;
    int err = MPI_Type_get_true_extent(MPI_CHAR, &lb, &esz);
    printErr(*piol, name, Log::Layer::Data, err, nullptr, "Setting MPI extent failed");

    if (esz != 1)
        piol->record(name, Log::Layer::Data, Log::Status::Error, "MPI_CHAR extent is bigger than one.", Log::Verb::None);

    fcomm = MPI_COMM_SELF;

    file = mopen(*piol, fcomm, opt, name);
    if (file != MPI_FILE_NULL)
    {
        int err = MPI_File_set_view(file, 0, MPI_CHAR, MPI_CHAR, "native", info);
        printErr(*piol, name, Log::Layer::Data, err, nullptr, "MPIIO Constructor failed to set a view");
    }
}

MPIIO::~MPIIO(void)
{
    if (file != MPI_FILE_NULL)
        MPI_File_close(&file);
}

///////////////////////////////////       Member functions      ///////////////////////////////////
size_t MPIIO::getFileSz() const
{
    MPI_Offset fsz;
    int err = MPI_File_get_size(file, &fsz);
    printErr(*piol, name, Log::Layer::Data, err, nullptr, "error getting the file size");
    return size_t(fsz);
}

void MPIIO::setFileSz(csize_t sz) const
{
    int err = MPI_File_preallocate(file, MPI_Offset(sz));
    printErr(*piol, name, Log::Layer::Data, err, nullptr, "error setting the file size");
}

void MPIIO::read(csize_t offset, csize_t sz, uchar * d) const
{
    MPI_Status arg;
    int err = io<uchar, MPI_Status>(MPI_File_read_at, file, offset, sz, arg, maxSize, d);
    printErr(*piol, name, Log::Layer::Data, err, &arg, " non-collective read Failure\n");
}

int strideView(MPI_File file, MPI_Info info, MPI_Offset offset, int block, MPI_Aint stride, int count, MPI_Datatype * type)
{
    int err = MPI_Type_create_hvector(count, block, stride, MPI_CHAR, type);
    if (err != MPI_SUCCESS)
        return err;

    err = MPI_Type_commit(type);
    if (err != MPI_SUCCESS)
        return err;

    err = MPI_File_set_view(file, offset, MPI_CHAR, *type, "native", info);
    return err;
}

//TODO: Currently this implementation has a known issue with big sizes.
//The idea is to get views working first, then address that.
void MPIIO::readSmall(csize_t offset, csize_t bsz, csize_t osz, csize_t nb, uchar * d) const
{
    if (nb*osz > size_t(maxSize))
    {
        std::string msg = "(nb, bsz, osz) = (" + std::to_string(nb) + ", "
                                               + std::to_string(bsz) + ", "
                                               + std::to_string(osz) + ")";
        piol->record(name, Log::Layer::Data, Log::Status::Error, "Read overflows MPI settings: " + msg, Log::Verb::None);
    }

    //Set a view so that MPI_File_read... functions only see contiguous data.
    MPI_Datatype view;
    int err = strideView(file, info, offset, bsz, osz, nb, &view);
    printErr(*piol, name, Log::Layer::Data, err, NULL, "Failed to set a view for reading.");

    MPI_Aint lb;
    MPI_Aint esz;

    MPIIO::read(0U, nb*bsz, d);

    //Reset the view.
    MPI_File_set_view(file, 0, MPI_CHAR, MPI_CHAR, "native", info);
    MPI_Type_free(&view);
}

constexpr size_t getFabricPacketSz(void)
{
    return 4U*1024U*1024U;
}

void MPIIO::read(csize_t offset, csize_t bsz, csize_t osz, csize_t nb, uchar * d) const
{
    /*
     *  If the bsz size is very large, we may as well read do this as a sequence of separate reads.
     *  If MPI_Aint ignores the spec, then we are also contrained to this.
     *  TODO: Investigate which limit is the optimal choice if the need arises
    */
    if (bsz > getFabricPacketSz() || (sizeof(MPI_Aint) < sizeof(size_t) && osz > maxSize))
        for (size_t i = 0; i < nb; i++)
            MPIIO::read(offset+i*osz, bsz, d);

    auto viewRead = [this, offset, bsz, osz]
        (MPI_File file, MPI_Offset off, void * d, int numb, MPI_Datatype da, MPI_Status * stat) -> int
        {
            readSmall(off, bsz, osz, size_t(numb), static_cast<uchar *>(d));
            return MPI_SUCCESS;
        };

    MPI_Status stat;
    int err = io<uchar, MPI_Status>(viewRead, file, offset, nb, stat, maxSize, d, bsz, osz);
    printErr(*piol, name, Log::Layer::Data, err, NULL, "Failed to read data over the integer limit.");
}

void MPIIO::write(csize_t offset, size_t sz, const uchar * d) const
{
    MPI_Status arg;
    int err = io<uchar, MPI_Status>(mpiio_write_at, file, offset, sz, arg, maxSize, const_cast<uchar *>(d));
    printErr(*piol, name, Log::Layer::Data, err, &arg, "Non-collective read failure.");
}
}}
