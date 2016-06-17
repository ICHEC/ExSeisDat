#ifndef PIOLCOMMMPI_INCLUDE_GUARD
#define PIOLCOMMMPI_INCLUDE_GUARD
#include "comm/comm.hh"
namespace PIOL { namespace Comms {

class MPI : public Comms::Interface
{
    private :
    MPI_Comm comm;
    bool init;
    public :
    MPI(MPI_Comm Comm, bool Init = true) : init(Init)
    {
        comm = Comm;
        if (init)
        {
            //Quote from MPI 3.1 specification: "The version for ISO C accepts the argc and argv that are provided by the arguments to main or NULL"
            int err = MPI_Init(NULL, NULL);
            Block::MPI::printErr(err, NULL, "MPI_Init failure\n"); 
        }
        int irank;
        int inumRank;
        MPI_Comm_rank(Comm, &irank);
        MPI_Comm_size(Comm, &inumRank);
        rank = irank;
        numRank = inumRank;
    }
    ~MPI(void)
    {
        if (init)
        {
            int err = MPI_Finalize();
            Block::MPI::printErr(err, NULL, "MPI_Finalize failure\n");
        }
    }
};

}}
#endif

