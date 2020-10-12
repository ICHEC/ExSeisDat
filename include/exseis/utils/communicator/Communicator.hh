////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The base communicator layer
/// @details The base class exists so that the ExSeisPIOL is not completely tied
///          to MPI.  This is useful for two reasons. Firstly, it means that MPI
///          specific work is localised in a specific portion of the code rather
///          than being distributed throughout and common functionality can be
///          grouped up, but also MPI could be switched for another
///          inter-process communication technology if one is of particular
///          interest.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_UTILS_COMMUNICATOR_COMMUNICATOR_HH
#define EXSEIS_UTILS_COMMUNICATOR_COMMUNICATOR_HH

#include "exseis/utils/logging/Log.hh"
#include "exseis/utils/types/typedefs.hh"

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace exseis {
inline namespace utils {
inline namespace communicator {

/// @brief Communication with other processes and the outside world
///
class Communicator {
  public:
    class Implementation;

  protected:
    /// A pointer to an instance of Implementation
    std::shared_ptr<const Implementation> m_implementation = nullptr;

  public:
    /// @brief Construct a Communicator from an Implementation
    /// @param[in] implementation The underlying Implementation to use
    Communicator(std::shared_ptr<const Implementation> implementation) :
        m_implementation(std::move(implementation))
    {
    }

    /// @brief Returns the rank of the process executing the function/
    ///
    /// @return The rank.
    ///
    size_t get_rank() const { return m_implementation->get_rank(); }

    /// @brief Returns the number of processes which are executing together.
    ///
    /// @return The number of processes (i.e number of ranks).
    ///
    size_t get_num_rank() const { return m_implementation->get_num_rank(); }

    /// @brief Pass a vector of double and return the corresponding values to
    ///        each process
    ///
    /// @param[in] val The local value to use in the gather
    ///
    /// @return Return a vector where the nth element is the value from the nth
    ///         rank.
    ///
    std::vector<double> gather(const std::vector<double>& val) const
    {
        return m_implementation->gather(val);
    }

    /// @brief Pass a vector of double and return the corresponding values to
    ///        each process
    ///
    /// @param[in] val The local value to use in the gather
    ///
    /// @return Return a vector where the nth element is the value from the nth
    ///         rank.
    ///
    std::vector<float> gather(const std::vector<float>& val) const
    {
        return m_implementation->gather(val);
    }

    /// @brief Pass a vector of Integer and return the
    ///        corresponding values to each process
    ///
    /// @param[in] val The local value to use in the gather
    ///
    /// @return Return a vector where the nth element is the value from the nth
    ///         rank.
    ///
    std::vector<Integer> gather(const std::vector<Integer>& val) const
    {
        return m_implementation->gather(val);
    }

    /// @brief Pass a vector of size_t and return the corresponding values to
    ///        each process
    ///
    /// @param[in] val The local value to use in the gather
    ///
    /// @return Return a vector where the nth element is the value from the nth
    ///         rank.
    ///
    std::vector<size_t> gather(const std::vector<size_t>& val) const
    {
        return m_implementation->gather(val);
    }

    /// @brief Pass a value and return the corresponding values to each process
    ///
    /// @tparam T The type use for the gather.
    ///
    /// @param[in] val  The local value to use in the gather
    ///
    /// @return Return a vector where the nth element is the value from the nth
    ///         rank.
    ///
    template<class T>
    std::vector<T> gather(const T& val) const
    {
        return gather(std::vector<T>{val});
    }

    /// @brief Perform a reduce across all process to get the sum of the passed
    ///        values
    ///
    /// @param[in] val variable to be used in the operation from this process
    ///
    /// @return the global sum (same value on all processes)
    ///
    size_t sum(size_t val) const { return m_implementation->sum(val); }

    /// @brief Perform a reduce across all process to get the max of the passed
    ///        values
    ///
    /// @param[in] val variable to be used in the operation from this process
    ///
    /// @return the global max (same value on all processes)
    ///
    size_t max(size_t val) const { return m_implementation->max(val); }

    /// @brief Perform a reduce across all process to get the min of the passed
    ///        values
    ///
    /// @param[in] val variable to be used in the operation from this process
    ///
    /// @return the global min (same value on all processes)
    ///
    size_t min(size_t val) const { return m_implementation->min(val); }

    /// @brief Calculate the offset assuming a local given contribution
    ///
    /// @param[in] val variable to be used in the operation from this process
    ///
    /// @return the local offset (equivalent to an MPI exscan)
    ///
    size_t offset(size_t val) const { return m_implementation->offset(val); }

    /// @brief A barrier between all processes which are members of the
    ///        communication collective.
    ///
    /// @details Implementations of this pure virtual function will perform a
    ///          collective wait.
    ///
    void barrier() const { m_implementation->barrier(); }

    /// @brief Get a logger appropriate for the communicator
    /// @returns A shared_ptr to a logger
    std::shared_ptr<Log> log() const { return m_implementation->log(); }

    /// @brief Polymorphic interface for the Communicator
    ///
    /// Classes implementing a Communicator will inherit from Communicator,
    /// and contain a class called Implementation which inherits from this.
    /// Calls to the Communicator class forward directly to a pointer of this
    /// class type.
    /// The implementation details of the deriving class should be placed
    /// primarily in this polymorphic interface.
    ///
    class Implementation {
      public:
        /// @virtual_destructor
        virtual ~Implementation() = default;

        /// @copydoc Communicator::get_rank
        virtual size_t get_rank() const = 0;

        /// @copydoc Communicator::get_num_rank
        virtual size_t get_num_rank() const = 0;

        /// @copydoc Communicator::gather(const std::vector<double>&) const
        virtual std::vector<double> gather(
            const std::vector<double>& val) const = 0;

        /// @copydoc Communicator::gather(const std::vector<float>&) const
        virtual std::vector<float> gather(
            const std::vector<float>& val) const = 0;

        /// @copydoc Communicator::gather(const std::vector<Integer>&) const
        virtual std::vector<Integer> gather(
            const std::vector<Integer>& val) const = 0;

        /// @copydoc Communicator::gather(const std::vector<size_t>&) const
        virtual std::vector<size_t> gather(
            const std::vector<size_t>& val) const = 0;

        /// @copydoc Communicator::sum
        virtual size_t sum(size_t val) const = 0;

        /// @copydoc Communicator::max
        virtual size_t max(size_t val) const = 0;

        /// @copydoc Communicator::min
        virtual size_t min(size_t val) const = 0;

        /// @copydoc Communicator::offset
        virtual size_t offset(size_t val) const = 0;

        /// @copydoc Communicator::barrier
        virtual void barrier() const = 0;

        /// @copydoc Communicator::log
        virtual std::shared_ptr<Log> log() const = 0;
    };
};

}  // namespace communicator
}  // namespace utils
}  // namespace exseis

#endif  // EXSEIS_UTILS_COMMUNICATOR_COMMUNICATOR_HH
