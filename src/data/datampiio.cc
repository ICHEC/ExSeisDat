#include "datampiio.hh"
#include "anc/piol.hh"
#include "anc/cmpi.hh"
#include "share/smpi.hh"

namespace PIOL { namespace Data {
MPI_File open(ExSeisPIOL & piol, const MPIIOOpt & opt, const std::string name)
{
    std::dynamic_pointer_cast<Comms::MPI>(piol.comm);
    MPI_Comm mcomm = std::dynamic_pointer_cast<Comms::MPI>(piol.comm)->getComm();

    MPI_File file = MPI_FILE_NULL;
    int err = MPI_File_open(mcomm, name.c_str(), opt.mode, opt.info, &file);

    printErr(piol, name, Log::Layer::Data, err, NULL, "MPI_File_open failure");

    //I assume this check condition is unnecessary but the spec does not explicitly say what state
    //file is in when there is an error.
    return (err != MPI_SUCCESS ? MPI_FILE_NULL : file);
}

MPIIO::MPIIO(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_, const MPIIOOpt & opt_) : PIOL::Data::Interface(piol_, name_), opt(opt_)
{
    file = open(*piol, opt, name);
    if (file != MPI_FILE_NULL)
        setView();
    else
    {
        piol->record(name, Log::Layer::Data, Log::Status::Error, "failed to open a file with MPI", Log::Verb::None);
        piol->exit(EXIT_FAILURE);
    }
}

MPIIO::~MPIIO(void)
{
    if (file != MPI_FILE_NULL)
        MPI_File_close(&file);
}

size_t MPIIO::getFileSz()
{
    MPI_Offset fsz;
    int err = MPI_File_get_size(file, &fsz);
    printErr(*piol, name, Log::Layer::Data, err, NULL, "error getting the file size");
    return size_t(fsz);
}

void MPIIO::setFileSz(const size_t sz)
{
    int err = MPI_File_set_size(file, MPI_Offset(sz));
    printErr(*piol, name, Log::Layer::Data, err, NULL, "error resizing the file");
}

}}
