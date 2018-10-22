////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author cathal o broin - cathal@ichec.ie - first commit
/// @copyright tbd. do not distribute
/// @date july 2016
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/CommunicatorMPI.hh"
#include "exseisdat/utils/mpi/MPI_error_to_string.hh"
#include "exseisdat/utils/mpi/MPI_type.hh"
#include "exseisdat/utils/typedefs.hh"

#include <cassert>
#include <limits>
#include <string>

using namespace std::string_literals;
using namespace exseis::utils::logging;

namespace exseis {
namespace piol {

// We define functions and classes to delegate initialization and finalization
// if MPI to initialization and destruction of function-local static variables.
// This means initialization is tied to the first piol::CommunicatorMPI call,
// and finalization is tied to the program exit.
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


CommunicatorMPI::CommunicatorMPI(
  exseis::utils::Log* log, const CommunicatorMPI::Opt& opt) :
    m_comm(opt.comm),
    m_log(log)
{
    // Initialize MPI and set up MPI_Finalize to be called at program close.
    MPIManager& mpi_manager = mpi_manager_instance();
    (void)mpi_manager;

    int irank;
    int inum_rank;
    MPI_Comm_rank(m_comm, &irank);
    MPI_Comm_size(m_comm, &inum_rank);

    // Check conversion to unsigned size_t works
    assert(irank >= 0);
    assert(inum_rank >= 0);

    m_rank     = static_cast<size_t>(irank);
    m_num_rank = static_cast<size_t>(inum_rank);
}

MPI_Comm CommunicatorMPI::get_comm() const
{
    return m_comm;
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
  exseis::utils::Log* log,
  const CommunicatorMPI* mpi,
  T val,
  MPI_Op op,
  exseis::utils::Source_position source_position)
{
    T result = 0;
    int err  = MPI_Allreduce(
      &val, &result, 1, exseis::utils::mpi_type<T>(), op, mpi->get_comm());

    if (err != MPI_SUCCESS) {
        log->add_entry(exseis::utils::Log_entry{
          exseis::utils::Status::Error,
          "MPI_Allreduce error: "s + exseis::utils::mpi_error_to_string(err),
          exseis::utils::Verbosity::none, source_position});
    }

    return (err == MPI_SUCCESS ? result : 0LU);
}
}  // namespace

size_t CommunicatorMPI::sum(size_t val) const
{
    return reduce_impl(
      m_log, this, val, MPI_SUM,
      EXSEISDAT_SOURCE_POSITION("exseis::piol::CommunicatorMPI::sum"));
}

size_t CommunicatorMPI::max(size_t val) const
{
    return reduce_impl(
      m_log, this, val, MPI_MAX,
      EXSEISDAT_SOURCE_POSITION("exseis::piol::CommunicatorMPI::max"));
}

size_t CommunicatorMPI::min(size_t val) const
{
    return reduce_impl(
      m_log, this, val, MPI_MIN,
      EXSEISDAT_SOURCE_POSITION("exseis::piol::CommunicatorMPI::min"));
}

size_t CommunicatorMPI::offset(size_t val) const
{
    size_t offset = 0LU;
    MPI_Exscan(
      &val, &offset, 1LU, exseis::utils::mpi_type<size_t>(), MPI_SUM,
      MPI_COMM_WORLD);
    return (m_rank == 0 ? 0LU : offset);
}


namespace {

/// @brief Implementation of \ref CommunicatorMPI::gather using MPI_Allgather.
///
/// @tparam T The datatype for the gather
///
/// @param[in] self            The `this` pointer of the CommunicatorMPI object.
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
  const CommunicatorMPI* self,
  const std::vector<T>& in,
  exseis::utils::Log* log,
  exseis::utils::Source_position source_position)
{
    std::vector<T> gathered_array(self->get_num_rank() * in.size());

    assert(in.size() < std::numeric_limits<int>::max());
    int int_in_size = static_cast<int>(in.size());
    int err         = MPI_Allgather(
      in.data(), int_in_size, exseis::utils::mpi_type<T>(),
      gathered_array.data(), int_in_size, exseis::utils::mpi_type<T>(),
      self->get_comm());

    if (err != MPI_SUCCESS) {
        log->add_entry(exseis::utils::Log_entry{
          exseis::utils::Status::Error,
          "MPI_Allgather error: "s + exseis::utils::mpi_error_to_string(err),
          exseis::utils::Verbosity::none, source_position});
    }

    return gathered_array;
}

}  // namespace

std::vector<exseis::utils::Integer> CommunicatorMPI::gather(
  const std::vector<exseis::utils::Integer>& in) const
{
    return gather_impl(
      this, in, m_log,
      EXSEISDAT_SOURCE_POSITION(
        "exseis::piol::CommunicatorMPI::gather<Integer>"));
}

std::vector<size_t> CommunicatorMPI::gather(const std::vector<size_t>& in) const
{
    return gather_impl(
      this, in, m_log,
      EXSEISDAT_SOURCE_POSITION(
        "exseis::piol::CommunicatorMPI::gather<size_t>"));
}

std::vector<float> CommunicatorMPI::gather(const std::vector<float>& in) const
{
    return gather_impl(
      this, in, m_log,
      EXSEISDAT_SOURCE_POSITION(
        "exseis::piol::CommunicatorMPI::gather<float>"));
}

std::vector<double> CommunicatorMPI::gather(const std::vector<double>& in) const
{
    return gather_impl(
      this, in, m_log,
      EXSEISDAT_SOURCE_POSITION(
        "exseis::piol::CommunicatorMPI::gather<double>"));
}


void CommunicatorMPI::barrier(void) const
{
    MPI_Barrier(m_comm);
}

}  // namespace piol
}  // namespace exseis
