////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author cathal o broin - cathal@ichec.ie - first commit
/// @date july 2016
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "exseis/utils/communicator/Communicator_mpi.hh"
#include "exseis/utils/mpi_utils/MPI_error_to_string.hh"
#include "exseis/utils/types/MPI_type.hh"
#include "exseis/utils/types/typedefs.hh"

#include <cassert>
#include <limits>
#include <string>


namespace exseis {
inline namespace utils {
inline namespace communicator {

// We define functions and classes to delegate initialization and finalization
// if MPI to initialization and destruction of function-local static variables.
// This means initialization is tied to the first
// exseis::utils::Communicator_mpi call, and finalization is tied to the program
// exit.
//
// We also allow the user to circumvent this behaviour with a global variable,
// set by calling manageMPI(bool).

namespace {

/// @brief An encoding whether we're explicitly managing or not managing MPI, or
///        if management has even been decided yet.
enum class ManagingMPI { unset, yes, no };

/// @brief A static variable tracking whether we're managing MPI or not.
///
///        By default it's "unset", but after MPIManager() is called, it will
///        definitely be set.
///
/// @return Return (a reference to) whether we're managing MPI or not.
///
ManagingMPI& managing_mpi()
{
    static auto managing_mpi = ManagingMPI::unset;
    return managing_mpi;
}


/// @brief A class managing the lifetime of the MPI library.
///
/// This class will be initialized as a function static variable.
/// This means the lifetime will begin the first time the MPIManagerInstance
/// function is called, and end when the program exits wither by returning from
/// main, or when std::exit() is called.  It uses the managingMPI() static
/// variable to track management, which users can explicitly set using the
/// manageMPI(bool) function.
///
struct MPIManager {
    /// @brief Initialize MPI if it hasn't been already, and we're responsible
    ///        for it.
    MPIManager()
    {
        // If we're not managing MPI, just do nothing.
        if (managing_mpi() == ManagingMPI::no) {
            return;
        }

        int initialized = 0;
        MPI_Initialized(&initialized);

        if (initialized == 0) {
            MPI_Init(nullptr, nullptr);
        }

        // Set managingMPI value if the user hasn't already
        if (managing_mpi() == ManagingMPI::unset) {
            if (initialized != 0) {
                // MPI was already initialized
                managing_mpi() = ManagingMPI::no;
            }
            else {
                // We initialized MPI
                managing_mpi() = ManagingMPI::yes;
            }
        }
    }

    /// @brief Finalize MPI if we're responsible for it.
    ~MPIManager()
    {
        if (managing_mpi() == ManagingMPI::yes) {
            int finalized = 0;
            MPI_Finalized(&finalized);

            if (finalized == 0) {
                MPI_Finalize();
            }
        }
    }
};


/// @brief A static instance of MPIManager so the destructor, and MPI_Finalize
///        will be called at program exit.
///
/// @return A reference to the static MPIManager instance.
///
MPIManager& mpi_manager_instance()
{
    static auto& managing_mpi_guard = managing_mpi();
    (void)managing_mpi_guard;
    static MPIManager mpi_manager_instance;
    return mpi_manager_instance;
}

}  // namespace


/// @brief Set whether ExSeisDat should manage the initialization / finalization
///        of the MPI library.
/// @param[in] manage Whether ExSeisDat should manage MPI.
///
void manage_mpi(bool manage)
{
    if (manage) {
        managing_mpi() = ManagingMPI::yes;
    }
    else {
        managing_mpi() = ManagingMPI::no;
    }
}


Communicator_mpi::Implementation::Implementation(
    MPI_Comm communicator, exseis::Verbosity verbosity) :
    m_communicator(communicator), m_log(std::make_shared<Log>(verbosity))
{
    // Initialize MPI and set up MPI_Finalize to be called at program close.
    MPIManager& mpi_manager = mpi_manager_instance();
    (void)mpi_manager;

    int irank;
    int inum_rank;
    MPI_Comm_rank(m_communicator, &irank);
    MPI_Comm_size(m_communicator, &inum_rank);

    // Check conversion to unsigned size_t works
    assert(irank >= 0);
    assert(inum_rank >= 0);

    m_rank     = static_cast<size_t>(irank);
    m_num_rank = static_cast<size_t>(inum_rank);
}

MPI_Comm Communicator_mpi::Implementation::mpi_communicator() const
{
    return m_communicator;
}

size_t Communicator_mpi::Implementation::get_rank() const
{
    return m_rank;
}

size_t Communicator_mpi::Implementation::get_num_rank() const
{
    return m_num_rank;
}


namespace {
/// @brief Implementation for `sum`, `min`, and `max` using MPI_Allreduce.
///
/// @tparam T The type of the values
///
/// @param[in,out] log The logging layer
/// @param[in] mpi The MPI communicator
/// @param[in] val The value to be reduced
/// @param[in] op The operation
/// @param[in] source_position The position in the source code to report to the
///                            logger. Set this to the point where this function
///                            is called.
///
/// @return Return the result of the reduce operation
///
template<typename T>
T reduce_impl(
    const std::shared_ptr<Log>& log,
    const Communicator_mpi::Implementation* mpi,
    T val,
    MPI_Op op,
    Source_position source_position)
{
    T result = 0;
    int err  = MPI_Allreduce(
        &val, &result, 1, mpi_type<T>(), op, mpi->mpi_communicator());

    if (err != MPI_SUCCESS) {
        log->add_entry(Log_entry{Status::Error,
                                 std::string{"MPI_Allreduce error: "}
                                     + mpi_error_to_string(err),
                                 Verbosity::none, source_position});
    }

    return (err == MPI_SUCCESS ? result : 0LU);
}
}  // namespace

size_t Communicator_mpi::Implementation::sum(size_t val) const
{
    return reduce_impl(
        m_log, this, val, MPI_SUM,
        EXSEIS_SOURCE_POSITION(
            "exseis::utils::Communicator_mpi::Implementation::sum"));
}

size_t Communicator_mpi::Implementation::max(size_t val) const
{
    return reduce_impl(
        m_log, this, val, MPI_MAX,
        EXSEIS_SOURCE_POSITION(
            "exseis::utils::Communicator_mpi::Implementation::max"));
}

size_t Communicator_mpi::Implementation::min(size_t val) const
{
    return reduce_impl(
        m_log, this, val, MPI_MIN,
        EXSEIS_SOURCE_POSITION(
            "exseis::utils::Communicator_mpi::Implementation::min"));
}

size_t Communicator_mpi::Implementation::offset(size_t val) const
{
    size_t offset = 0LU;
    MPI_Exscan(&val, &offset, 1LU, mpi_type<size_t>(), MPI_SUM, MPI_COMM_WORLD);
    return (m_rank == 0 ? 0LU : offset);
}


namespace {

/// @brief Implementation of \ref Communicator_mpi::Implementation::gather using MPI_Allgather.
///
/// @tparam T The datatype for the gather
///
/// @param[in] self            The `this` pointer of the Communicator_mpi object.
/// @param[in] in              The local value to use in the gather
/// @param[in] log             The Log instance to log to.
/// @param[in] source_position The position in the source code to report to the
///                            logger. Set this to the point where this function
///                            is called.
///
/// @return Return a vector where the nth element is the value from the nth
///         rank.
///
template<typename T>
std::vector<T> gather_impl(
    const Communicator_mpi::Implementation* self,
    const std::vector<T>& in,
    const std::shared_ptr<Log>& log,
    Source_position source_position)
{
    std::vector<T> gathered_array(self->get_num_rank() * in.size());

    assert(in.size() < std::numeric_limits<int>::max());
    int int_in_size = static_cast<int>(in.size());
    int err         = MPI_Allgather(
        in.data(), int_in_size, mpi_type<T>(), gathered_array.data(),
        int_in_size, mpi_type<T>(), self->mpi_communicator());

    if (err != MPI_SUCCESS) {
        log->add_entry(Log_entry{Status::Error,
                                 std::string{"MPI_Allgather error: "}
                                     + mpi_error_to_string(err),
                                 Verbosity::none, source_position});
    }

    return gathered_array;
}

}  // namespace

std::vector<Integer> Communicator_mpi::Implementation::gather(
    const std::vector<Integer>& in) const
{
    return gather_impl(
        this, in, m_log,
        EXSEIS_SOURCE_POSITION(
            "exseis::utils::Communicator_mpi::Implementation::gather<Integer>"));
}

std::vector<size_t> Communicator_mpi::Implementation::gather(
    const std::vector<size_t>& in) const
{
    return gather_impl(
        this, in, m_log,
        EXSEIS_SOURCE_POSITION(
            "exseis::utils::Communicator_mpi::Implementation::gather<size_t>"));
}

std::vector<float> Communicator_mpi::Implementation::gather(
    const std::vector<float>& in) const
{
    return gather_impl(
        this, in, m_log,
        EXSEIS_SOURCE_POSITION(
            "exseis::utils::Communicator_mpi::Implementation::gather<float>"));
}

std::vector<double> Communicator_mpi::Implementation::gather(
    const std::vector<double>& in) const
{
    return gather_impl(
        this, in, m_log,
        EXSEIS_SOURCE_POSITION(
            "exseis::utils::Communicator_mpi::Implementation::gather<double>"));
}


void Communicator_mpi::Implementation::barrier() const
{
    MPI_Barrier(m_communicator);
}

std::shared_ptr<Log> Communicator_mpi::Implementation::log() const
{
    return m_log;
}

}  // namespace communicator
}  // namespace utils
}  // namespace exseis
