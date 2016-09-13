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

/*! Retrieve the corresponding values from every process in a collective call
 * \param[in] mpi The MPI communication class
 * \param[in] in The local value to use in the gather
 * \tparam T The datatype for the gather
 * \return Return a vector where the nth element is the value from the nth rank.
 */
template <typename T>
std::vector<T> MPIGather(const MPI * mpi, const std::vector<T> & in)
{
    size_t isz = in.size();
    std::vector<T> arr(mpi->getNumRank() * in.size());
    int err = MPI_Allgather(in.data(), in.size(), MPIType<T>(), arr.data(), in.size(), MPIType<T>(), mpi->getComm());

    if (err != MPI_SUCCESS)
        std::cerr << "Allgather error\n";
    return arr;
}

inline
std::vector<llint> MPI::gather(const std::vector<llint> & in) const
{
    return MPIGather(this, in);
}

inline
std::vector<size_t> MPI::gather(const std::vector<size_t> & in) const
{
    return MPIGather(this, in);
}

inline
std::vector<geom_t> MPI::gather(const std::vector<geom_t> & in) const
{
    return MPIGather(this, in);
}

void MPI::barrier(void) const
{
    MPI_Barrier(comm);
}

Interface * MPI::subcomm(bool active)
{
    MPI_Comm newcomm;
    MPI_Group cgroup, ngroup;

    auto list = gather(std::vector<size_t>{active});
    std::vector<int> ranks;
    for (size_t i = 0; i < list.size(); i++)
        if (list[i] == true)
            ranks.push_back(i);

    MPI_Comm_group(comm, &cgroup);
    MPI_Group_incl(cgroup, ranks.size(), ranks.data(), &ngroup);
    MPI_Comm_create(comm, ngroup, &newcomm);
    MPI_Group_free(&cgroup);
    MPI_Group_free(&ngroup);

    MPIOpt opt;
    opt.initMPI = false;
    opt.comm = newcomm;
    return new MPI(opt);
}
}}
