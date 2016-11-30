/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include <assert.h>
#include "global.hh"
#include "anc/mpi.hh"
#include "share/mpi.hh"
namespace PIOL { namespace Comm {
MPI::MPI(Log::Logger * log_, const MPI::Opt & opt) : comm(opt.comm), init(opt.initMPI), log(log_)
{
    if (init)
    {
        //Quote from MPI 3.1 specification: "The version for ISO C accepts the argc and argv
        // that are provided by the arguments to main or NULL"
        int err = MPI_Init(NULL, NULL);
        printErr(log, "", Log::Layer::Comm, err, NULL, "MPI_Init failure");
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
        printErr(log, "", Log::Layer::Comm, err, NULL, "MPI_Finalize failure");
    }
}

MPI_Comm MPI::getComm() const
{
    return comm;
}

//Reduction for fundamental datatypes

/*! Retrieve the corresponding values from every process in a collective call
 * \tparam T The datatype for the gather
 * \param[in] log The ExSeisPIOL logger object
 * \param[in] mpi The MPI communication object
 * \param[in] in The local value to use in the gather
 * \return Return a vector where the nth element is the value from the nth rank.
 */
template <typename T>
std::vector<T> MPIGather(Log::Logger * log, const MPI * mpi, const std::vector<T> & in)
{
    std::vector<T> arr(mpi->getNumRank() * in.size());
    int err = MPI_Allgather(in.data(), in.size(), MPIType<T>(), arr.data(), in.size(), MPIType<T>(), mpi->getComm());
    printErr(log, "", Log::Layer::Comm, err, NULL, "MPI_Allgather failure");
    return arr;
}

/*! Perform a reduction with the specified operation.
 *  \tparam T The type of the values
 *  \param[in,out] log The logging layer
 *  \param[in] mpi The MPI communicator
 *  \param[in] val The value to be reduced
 *  \param[in] op The operation
 *  \return Return the result of the reduce operation
 */
template <typename T>
T getMPIOp(Log::Logger * log, const MPI * mpi, T val, MPI_Op op)
{
    T result = 0;
    int err = MPI_Allreduce(&val, &result, 1, MPIType<T>(), op, mpi->getComm());
    printErr(log, "", Log::Layer::Comm, err, NULL, "MPI_Allreduce failure");
    return (err == MPI_SUCCESS ? result : 0U);
}

size_t MPI::sum(size_t val)
{
    return getMPIOp(log, this, val, MPI_SUM);
}

size_t MPI::max(size_t val)
{
    return getMPIOp(log, this, val, MPI_MAX);
}

std::vector<llint> MPI::gather(const std::vector<llint> & in) const
{
    return MPIGather(log, this, in);
}

std::vector<size_t> MPI::gather(const std::vector<size_t> & in) const
{
    return MPIGather(log, this, in);
}

std::vector<geom_t> MPI::gather(const std::vector<geom_t> & in) const
{
    return MPIGather(log, this, in);
}

/*std::vector<size_t> MPI::gather(const std::vector<size_t> & in, size_t i) const
{
    std::vector<int32_t> lsz;
    {
        auto tsz = gather(std::vector<size_t>{in.size()});
        lsz.resize(tsz.size());
        for (size_t i = 0; i < tsz.size(); i++)
            lsz[i] = tsz[i];
    }
    size_t tots = lsz[0];

    std::vector<int32_t> displ(lsz.size());
    displ[0U] = 0U;
    for (size_t i = 1U; i < displ.size(); i++)
    {
        displ[i] = lsz[i-1];
        tots += lsz[i-1];
    }
    assert(tots < getLim<int32_t>()); //This gather can not handle MPI memory limits correctly.

    std::vector<size_t> out;
    int err;
    if (i != rank)
        err = MPI_Gatherv(in.data(), in.size(), MPIType<size_t>(), NULL, lsz.data(),
                                  displ.data(), MPIType<size_t>(), i, comm);
    else
    {
        out.resize(tots);
        err = MPI_Gatherv(in.data(), in.size(), MPIType<size_t>(), out.data(), lsz.data(),
                                  displ.data(), MPIType<size_t>(), i, comm);
    }
    printErr(log, "", Log::Layer::Set, err, NULL, "MPI_Allgatherv rcv failed");
    return out;
}*/

void MPI::barrier(void) const
{
    MPI_Barrier(comm);
}
}}
