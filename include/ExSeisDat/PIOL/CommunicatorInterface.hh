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
#ifndef EXSEISDAT_PIOL_COMMUNICATORINTERFACE_HH
#define EXSEISDAT_PIOL_COMMUNICATORINTERFACE_HH

#include "ExSeisDat/utils/typedefs.h"

#include <vector>

namespace exseis {
namespace PIOL {

using namespace exseis::utils::typedefs;

/*! @brief The Communication layer interface. Specific communication
 *         implementations work off this base class.
 */
class CommunicatorInterface {
  protected:
    /// A number in the sequence from 0 to some maximum (numRank-1) which
    /// indicates the process number.
    size_t rank;

    /// The total number of processes which are executing together.
    size_t numRank;

  public:
    /*! @brief A virtual destructor to allow deletion.
     */
    virtual ~CommunicatorInterface(void) = default;

    /*! @brief Returns the rank of the process executing the function/
     *  @return The rank.
     */
    virtual size_t getRank() const { return rank; }
    /*! @brief Returns the number of processes which are executing together.
     *  @return The number of processes (i.e number of ranks).
     */
    virtual size_t getNumRank() const { return numRank; }

    /*! @brief Pass a vector of double and return the corresponding values to
     *         each process
     *  @param[in] val The local value to use in the gather
     *  @return Return a vector where the nth element is the value from the nth
     *          rank.
     */
    virtual std::vector<double> gather(
      const std::vector<double>& val) const = 0;

    /*! @brief Pass a vector of double and return the corresponding values to
     *         each process
     *  @param[in] val The local value to use in the gather
     *  @return Return a vector where the nth element is the value from the nth
     *          rank.
     */
    virtual std::vector<float> gather(const std::vector<float>& val) const = 0;

    /*! @brief Pass a vector of exseis::utils::Integer and return the
     * corresponding values to each process
     *  @param[in] val The local value to use in the gather
     *  @return Return a vector where the nth element is the value from the nth
     *          rank.
     */
    virtual std::vector<exseis::utils::Integer> gather(
      const std::vector<exseis::utils::Integer>& val) const = 0;

    /*! @brief Pass a vector of size_t and return the corresponding values to
     *         each process
     *  @param[in] val The local value to use in the gather
     *  @return Return a vector where the nth element is the value from the nth
     *          rank.
     */
    virtual std::vector<size_t> gather(
      const std::vector<size_t>& val) const = 0;

    /*! @brief Pass a value and return the corresponding values to each process
     *  @tparam T The type use for the gather.
     *  @param[in] val The local value to use in the gather
     *  @return Return a vector where the nth element is the value from the nth
     *          rank.
     */
    template<class T>
    std::vector<T> gather(const T& val) const
    {
        return gather(std::vector<T>{val});
    }

    /*! Perform a reduce across all process to get the sum of the passed values
     *  @param[in] val variable to be used in the operation from this process
     *  @return the global sum (same value on all processes)
     */
    virtual size_t sum(size_t val) = 0;

    /*! Perform a reduce across all process to get the max of the passed values
     *  @param[in] val variable to be used in the operation from this process
     *  @return the global max (same value on all processes)
     */
    virtual size_t max(size_t val) = 0;

    /*! Perform a reduce across all process to get the min of the passed values
     *  @param[in] val variable to be used in the operation from this process
     *  @return the global min (same value on all processes)
     */
    virtual size_t min(size_t val) = 0;

    /*! Calculate the offset assuming a local given contribution
     *  @param[in] val variable to be used in the operation from this process
     *  @return the local offset (equivalent to an MPI exscan)
     */
    virtual size_t offset(size_t val) = 0;

    /*! @brief A barrier between all processes which are members of the
     *         communication collective.
     *         Implementations of this pure virtual function will perform a
     *         collective wait.
     */
    virtual void barrier(void) const = 0;
};

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_COMMUNICATORINTERFACE_HH
