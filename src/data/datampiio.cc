/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "datampiio.hh"
#include "anc/piol.hh"
#include "anc/cmpi.hh"
#include "share/smpi.hh"
namespace PIOL { namespace Data {
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////       Non-Class       ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
static MPI_File open(ExSeisPIOL & piol, MPI_Comm comm, const MPIIOOpt & opt, const std::string name)
{
    MPI_File file = MPI_FILE_NULL;
    int err = MPI_File_open(comm, name.c_str(), opt.mode, opt.info, &file);

    printErr(piol, name, Log::Layer::Data, err, nullptr, "MPI_File_open failure");

    //I assume this check condition is unnecessary but the spec does not explicitly say what state
    //file is in when there is an error.
    return (err != MPI_SUCCESS ? MPI_FILE_NULL : file);
}

/*! \brief Set an MPI File view on the file.
 *  \tparam T The C++ equivalent for the elementary datatype (see MPI spec)
 *  \tparam U The C++ equivalent for the file datatype (see MPI spec)
 *  \param[in] file The MPI file handle
 *  \param[in] offset The offset from the beginning of the file.
 *  \return Returns the MPI error code from MPI_File_set_view
 */
template <typename T, typename U=T>
int setView(const MPI_File file, const MPI_Offset offset = 0)
{
    MPI_Info info = MPI_INFO_NULL;
    int err = MPI_File_set_view(file, offset, MPIType<T>(), MPIType<U>(), "native", info);
    return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    Class functions    ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////      Constructor & Destructor      ///////////////////////////////
MPIIOOpt::MPIIOOpt(void)
{
    mode = MPI_MODE_RDONLY | MPI_MODE_UNIQUE_OPEN;
    info = MPI_INFO_NULL;
    maxSize = getLim<int32_t>();
}

MPIIO::MPIIO(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const MPIIOOpt & opt) : PIOL::Data::Interface(piol_, name_)
{
    maxSize = opt.maxSize;
    info = opt.info;

    auto mcomm = std::dynamic_pointer_cast<Comm::MPI>(piol->comm);
    if (mcomm == nullptr)
    {
        piol->record(name, Log::Layer::Data, Log::Status::Error, "Cast of communicator to MPI communicator failed", Log::Verb::None);
        return;
    }
    comm = mcomm->getComm();

    file = open(*piol, comm, opt, name);
    if (file != MPI_FILE_NULL)
    {
        int err = setView<uchar>(file);
        printErr(*piol, name, Log::Layer::Data, err, nullptr, "MPIIO Constructor failed to set a view");
    }
}

MPIIO::~MPIIO(void)
{
    if (file != MPI_FILE_NULL)
        MPI_File_close(&file);
}

///////////////////////////////////       Member functions      ///////////////////////////////////
size_t MPIIO::getFileSz()
{
    MPI_Offset fsz;
    int err = MPI_File_get_size(file, &fsz);
    printErr(*piol, name, Log::Layer::Data, err, nullptr, "error getting the file size");
    return size_t(fsz);
}

void MPIIO::setFileSz(const size_t sz)
{
    int err = MPI_File_preallocate(file, MPI_Offset(sz));
    printErr(*piol, name, Log::Layer::Data, err, nullptr, "error setting the file size");
}

/*! \brief This templated function pointer type allows us to refer to MPI functions more compactly.
 */
template <typename U>
using MFp = int (*)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, U *);

/*! \brief The MPI-IO inline template function for reading and writing
 *  \tparam T The type of the array being read to or from.
 *  \tparam U The type of the last argument of MPI_File_...
 *  \param[in] fn Function pointer to the MPI function.
 *  \param[in] file The MPI File
 *  \param[in] offset The offset from the current shared ptr
 *  \param[out] d The array to read into or from
 *  \param[in] sz The number of elements to read or write
 *  \param[in] arg The last argument of the MPI_File_... function
 *  \param[in] max The maximum size to read or write at once
 *  \return Returns the MPI error code. MPI_SUCCESS for success,
 *  MPI_ERR_IN_STATUS means the status structure should be checked.
 *
 * This function does not currently take into account collective MPI calls
 * which would have issues with the number of operations being the same for each process
 */
template <typename T, typename U = MPI_Status> inline
int io(const MFp<U> fn, const MPI_File & file, const size_t offset, T * d, const size_t sz, U & arg, const size_t max)
{
    int err = MPI_SUCCESS;
    auto q = sz / max;
    auto r = sz % max;

    for (auto i = 0U; i < q; i++)
    {
        err = fn(file, MPI_Offset(offset + i*max), &d[i*max], max, MPIType<T>(), &arg);
        if (err != MPI_SUCCESS)
            break;
    }

    if (err == MPI_SUCCESS)
        err = fn(file, MPI_Offset(offset + q*max), &d[q*max], r, MPIType<T>(), &arg);

    return err;
}

void MPIIO::read(const size_t offset, const size_t sz, uchar * d)
{
    MPI_Status arg;
    int err = io<uchar, MPI_Status>(MPI_File_read_at, file, offset, d, sz, arg, maxSize);
    printErr(*piol, name, Log::Layer::Data, err, &arg, " non-collective read Failure\n");
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

void MPIIO::write(const size_t offset, size_t sz, const uchar * d)
{
    MPI_Status arg;
    int err = io<uchar, MPI_Status>(mpiio_write_at, file, offset, const_cast<uchar *>(d), sz, arg, maxSize);
    printErr(*piol, name, Log::Layer::Data, err, &arg, " non-collective read Failure\n");
}


}}
