/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
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
            std::exit(EXIT_FAILURE);
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

MPI_Comm MPI::getComm() const
{
    return comm;
}

//Reduction for fundamental datatypes
template <typename T>
std::vector<T> MPIGather(const MPI * mpi, T val)
{
    std::vector<T> arr(mpi->getNumRank());
    arr[mpi->getRank()] = val;

    //TODO: Non-blocking target to be combined with other operations.
    int err = MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, arr.data(), 1, MPIType<T>(), mpi->getComm());
    if (err != MPI_SUCCESS)
        std::cerr << "Allgather error\n";
    return arr;
}

std::vector<llint> MPI::gather(llint val) const
{
    return MPIGather(this, val);
}

std::vector<size_t> MPI::gather(size_t val) const
{
    return MPIGather(this, val);
}

std::vector<geom_t> MPI::gather(geom_t val) const
{
    return MPIGather(this, val);
}

void MPI::barrier(void) const
{
    MPI_Barrier(comm);
}
}}
