////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author cathal o broin - cathal@ichec.ie - first commit
/// @copyright tbd. do not distribute
/// @date july 2016
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/anc/mpi.hh"
#include "ExSeisDat/PIOL/global.hh"
#include "ExSeisDat/PIOL/share/mpi.hh"

namespace PIOL {
namespace Comm {

// We define functions and classes to delegate initialization and finalization
// if MPI to initialization and destruction of function-local static variables.
// This means initialization is tied to the first PIOL::Comm::MPI call,
// and finalization is tied to the program exit.
//
// We also allow the user to circumvent this behaviour with a global variable,
// set by calling manageMPI(bool).

namespace {

/// An enum class encoding whether we're explicitly managing or not managing
/// MPI, or if management has even been decided yet.
enum class ManagingMPI { unset, yes, no };

/// A static variable tracking whether we're managing MPI or not.
/// By default it's "unset", but after MPIManager() is called, it will
/// definitely be set.
/// @return Return (a reference to) whether we're managing MPI or not.
ManagingMPI& managingMPI()
{
    static auto managingMPI = ManagingMPI::unset;
    return managingMPI;
}


/// @brief This class will be initialized as a function static variable.
///     This means the lifetime will begin the first time the MPIManagerInstance
///     function is called, and end when the program exits wither by returning
///     from main, or when std::exit() is called.
///     It uses the managingMPI() static variable to track management, which
///     users can explicitly set using the manageMPI(bool) function.
struct MPIManager {
    /// @brief Initialize MPI if it hasn't been already, and we're responsible
    ///     for it.
    MPIManager()
    {
        // If we're not managing MPI, just do nothing.
        if (managingMPI() == ManagingMPI::no) return;

        int initialized = 0;
        MPI_Initialized(&initialized);

        if (!initialized) MPI_Init(NULL, NULL);

        // Set managingMPI value if the user hasn't already
        if (managingMPI() == ManagingMPI::unset) {
            if (initialized) {
                // MPI was already initialized
                managingMPI() = ManagingMPI::no;
            }
            else {
                // We initialized MPI
                managingMPI() = ManagingMPI::yes;
            }
        }
    }

    /// @brief Finalize MPI if we're responsible for it.
    ~MPIManager()
    {
        if (managingMPI() == ManagingMPI::yes) {
            int finalized = false;
            MPI_Finalized(&finalized);

            if (!finalized) {
                MPI_Finalize();
            }
        }
    }
};


/// @brief A static instance of MPIManager so the destructor, and MPI_Finalize
///        will be called at program exit.
/// @return A reference to the static MPIManager instance.
MPIManager& MPIManagerInstance()
{
    static auto& managing_mpi = managingMPI();
    (void)managing_mpi;
    static auto MPIManagerInstance = MPIManager();
    return MPIManagerInstance;
}

}  // namespace


/// @brief Set whether ExSeisDat should manage the initialization / finalization
///        of the MPI library.
/// @param[in] manage Whether ExSeisDat should manage MPI.
void manageMPI(bool manage)
{
    if (manage) {
        managingMPI() = ManagingMPI::yes;
    }
    else {
        managingMPI() = ManagingMPI::no;
    }
}


MPI::MPI(Log::Logger* log_, const MPI::Opt& opt) : comm(opt.comm), log(log_)
{
    // Initialize MPI and set up MPI_Finalize to be called at program close.
    MPIManager& mpi_manager = MPIManagerInstance();
    (void)mpi_manager;

    int irank;
    int inumRank;
    MPI_Comm_rank(comm, &irank);
    MPI_Comm_size(comm, &inumRank);
    rank    = irank;
    numRank = inumRank;
}

MPI_Comm MPI::getComm() const
{
    return comm;
}

// Reduction for fundamental datatypes

/*! Retrieve the corresponding values from every process in a collective call
 * @tparam T The datatype for the gather
 * @param[in] log The ExSeisPIOL logger object
 * @param[in] mpi The MPI communication object
 * @param[in] in The local value to use in the gather
 * @return Return a vector where the nth element is the value from the nth rank.
 */
template<typename T>
std::vector<T> MPIGather(
  Log::Logger* log, const MPI* mpi, const std::vector<T>& in)
{
    std::vector<T> arr(mpi->getNumRank() * in.size());
    int err = MPI_Allgather(
      in.data(), in.size(), MPIType<T>(), arr.data(), in.size(), MPIType<T>(),
      mpi->getComm());
    printErr(log, "", Log::Layer::Comm, err, NULL, "MPI_Allgather failure");
    return arr;
}

/*! Perform a reduction with the specified operation.
 *  @tparam T The type of the values
 *  @param[in,out] log The logging layer
 *  @param[in] mpi The MPI communicator
 *  @param[in] val The value to be reduced
 *  @param[in] op The operation
 *  @return Return the result of the reduce operation
 */
template<typename T>
T getMPIOp(Log::Logger* log, const MPI* mpi, T val, MPI_Op op)
{
    T result = 0;
    int err = MPI_Allreduce(&val, &result, 1, MPIType<T>(), op, mpi->getComm());
    printErr(log, "", Log::Layer::Comm, err, NULL, "MPI_Allreduce failure");
    return (err == MPI_SUCCESS ? result : 0LU);
}

size_t MPI::sum(size_t val)
{
    return getMPIOp(log, this, val, MPI_SUM);
}

size_t MPI::max(size_t val)
{
    return getMPIOp(log, this, val, MPI_MAX);
}

size_t MPI::min(size_t val)
{
    return getMPIOp(log, this, val, MPI_MIN);
}

size_t MPI::offset(size_t val)
{
    size_t offset = 0LU;
    MPI_Exscan(&val, &offset, 1LU, MPIType<size_t>(), MPI_SUM, MPI_COMM_WORLD);
    return (!rank ? 0LU : offset);
}

std::vector<llint> MPI::gather(const std::vector<llint>& in) const
{
    return MPIGather(log, this, in);
}

std::vector<size_t> MPI::gather(const std::vector<size_t>& in) const
{
    return MPIGather(log, this, in);
}

std::vector<float> MPI::gather(const std::vector<float>& in) const
{
    return MPIGather(log, this, in);
}

std::vector<double> MPI::gather(const std::vector<double>& in) const
{
    return MPIGather(log, this, in);
}

void MPI::barrier(void) const
{
    MPI_Barrier(comm);
}

}  // namespace Comm
}  // namespace PIOL
