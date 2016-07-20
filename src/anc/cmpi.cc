#include "global.hh"
#include "anc/cmpi.hh"
#include "share/smpi.hh"
#include <iostream>
namespace PIOL { namespace Comm {
MPI::MPI(const MPIOpt & opt) : comm(opt.comm), init(opt.initMPI)
{
    if (init)
    {
        //Quote from MPI 3.1 specification: "The version for ISO C accepts the argc and argv
        // that are provided by the arguments to main or NULL"
        int err = MPI_Init(NULL, NULL);
        if (err != MPI_SUCCESS)
        {
            std::cerr << "MPI_Init failure\n";
            std::exit(-1);
        }
    }
    int irank;
    int inumRank;
    MPI_Comm_rank(comm, &irank);
    MPI_Comm_size(comm, &inumRank);
    rank = irank;
    numRank = inumRank;
}

MPI::~MPI(void)
{
    if (init)
    {
        int err = MPI_Finalize();
        if (err != MPI_SUCCESS)
        {
            std::cerr << "MPI_Finalize failure\n";
            std::exit(-1);
        }
    }
}
MPI_Comm MPI::getComm()
{
    return comm;
}

void MPI::barrier(void)
{
    MPI_Barrier(comm);
}
}}
