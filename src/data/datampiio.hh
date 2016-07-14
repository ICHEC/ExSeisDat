#ifndef PIOLDATAMPIIO_INCLUDE_GUARD
#define PIOLDATAMPIIO_INCLUDE_GUARD
#include <mpi.h>
#include <memory>
#include <typeinfo>
#include "global.hh"
#include "anc/piol.hh"
#include "anc/cmpi.hh"
#include "data/data.hh"

namespace PIOL { namespace Data {
template <typename U>
using FpR = int (*)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, U *);

template <typename U>
using FpW = int (*)(MPI_File, MPI_Offset, const void *, int, MPI_Datatype, U *);

/*enum class MPIIOModes
{
    NonCol,
    Col,
    Size
}*/

class MPIIOOpt : public Opt
{
    public :
/*    std::vector<FpR<MPI_Status>> blockReads;
    std::vector<FpW<MPI_Status>> blockWrites;
    std::vector<FpR<MPI_Request>> AsyncReads;
    std::vector<FpW<MPI_Request>> AsyncWrites;*/
    int mode;
    MPI_Info info;

    MPIIOOpt(void)
    {
        info = MPI_INFO_NULL;
/*        size_t sz = static_cast<size_t>(MPIIOModes::Size);
        reads.resize(sz);
        reads[MPIIOModes::Block] = MPI_File_read_at;
        reads[MPIIOModes::Async] = MPI_File_iread_at;
        reads[MPIIOModes::BlockCol] = MPI_File_read_at_all;
        reads[MPIIOModes::AsyncCol] = MPI_File_;

        writes.resize(sz);
        writes[MPIIOModes::Block] = mpiio_write_at;
        writes[MPIIOModes::Async] = MPI_File_iwrite_at;
        writes[MPIIOModes::BlockCol] = MPI_File_write_at_all;
        writes[MPIIOModes::AsyncCol] = MPI_File_iwrite_at_all;*/
    }
};

class MPIIO : public Interface
{
    MPI_File file;
    MPI_Comm mcomm;
    MPIIOOpt opt;
    public :
    MPIIO(std::shared_ptr<ExSeisPIOL> piol_, std::string name_, const MPIIOOpt & opt);

    ~MPIIO(void);

    size_t getFileSz();

    void read(size_t offset, uchar * d, size_t sz);
};

}}
#endif
