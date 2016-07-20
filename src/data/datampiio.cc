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

template <typename T, typename U=T>
int setView(MPI_File file, MPI_Offset offset = 0)
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
    maxSize = getLim<long long>();
}

MPIIO::MPIIO(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const MPIIOOpt & opt) : PIOL::Data::Interface(piol_, name_)
{
    maxSize = opt.maxSize;
    info = opt.info;

    std::dynamic_pointer_cast<Comm::MPI>(piol->comm);
    comm = std::dynamic_pointer_cast<Comm::MPI>(piol->comm)->getComm();

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

template <typename T, typename U = MPI_Status> inline
int MPIIORead(FpR<U> fn, MPI_File & file, size_t offset, T * d, size_t sz, U & arg, size_t max)
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

void MPIIO::read(size_t offset, uchar * d, size_t sz)
{
    MPI_Status arg;
    int err = MPIIORead<uchar, MPI_Status>(MPI_File_read_at, file, offset, d, sz, arg, maxSize);
    printErr(*piol, name, Log::Layer::Data, err, &arg, " non-collective read Failure\n");
}
}}
