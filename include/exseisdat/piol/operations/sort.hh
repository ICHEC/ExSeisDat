////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The Sort Operation
/// @details The algorithm used is a nearest neighbour approach where at each
///          iteration the lowest valued metadata entries are moved to adjacent
///          processes with a lower rank and a sort is performed. After the sort
///          the highest entries are moved again to the process with a higher
///          rank. If each process has the same traces it started off with, the
///          sort is complete.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_OPERATIONS_SORT_HH
#define EXSEISDAT_PIOL_OPERATIONS_SORT_HH

#include "exseisdat/piol/ExSeisPIOL.hh"
#include "exseisdat/piol/ReadInterface.hh"
#include "exseisdat/piol/Trace_metadata.hh"
#include "exseisdat/piol/operations/SortType.hh"
#include "exseisdat/utils/decomposition/block_decomposition.hh"
#include "exseisdat/utils/typedefs.hh"

#include <functional>

namespace exseis {
namespace piol {

/// A template for the Compare less-than function
template<class T>
using Compare = std::function<bool(const T&, const T&)>;

/// A template for the Compare less-than function
typedef std::function<bool(const Trace_metadata&, const size_t, const size_t)>
    CompareP;

/************************************ Core ************************************/

/*! Get the sorted index associated with a given list (support function)
 *  @param[in] sz The length of the list
 *  @param[in] list The array of numbers
 *  @return A vector containing the numbering of list in a sorted order
 */
std::vector<size_t> get_sort_index(size_t sz, const size_t* list);

/*! Function to sort the metadata in a Trace_metadata struct. The returned
 * vector is the location where the nth parameter is located in the sorted list.
 *  Implementation note: the Trace_metadata vector is used internally to allow
 *  random-access iterator support.
 *  @param[in] piol The PIOL object.
 *  @param[in,out] prm The parameter structure to sort
 *  @param[in] comp The Trace_metadata function to use for less-than comparisons between
 *                  objects in the vector. It assumes each Trace_metadata
 * structure has exactly one entry.
 *  @param[in] file_order Do we wish to have the sort in the sorted input order
 *                       (true) or sorted order (false)
 *  @return Return the correct order of traces from those which are smallest
 *          with respect to the comp function.
 */
std::vector<size_t> sort(
    ExSeisPIOL* piol,
    Trace_metadata& prm,
    CompareP comp,
    bool file_order = true);

/********************************** Non-Core **********************************/
/*! Perform a sort on the given parameter structure.
 *  @param[in] piol The PIOL object
 *  @param[in] type The sort type
 *  @param[in,out] prm The trace parameter structure.
 *  @return Return a vector which is a list of the ordered trace numbers. i.e
 *          the 0th member is the position of the 0th trace post-sort.
 */
std::vector<size_t> sort(ExSeisPIOL* piol, SortType type, Trace_metadata& prm);

/*! Check that the file obeys the expected ordering.
 *  @param[in] src The input file.
 *  @param[in] dec The decomposition: a pair which contains the \c offset
 *                 and the number of traces (\c size) for the local process.
 *  @param[in] type The sort type
 *  @return Return true if the local ordering is correct.
 */
bool check_order(
    const ReadInterface& src,
    exseis::utils::Contiguous_decomposition dec,
    SortType type);

/*! Return the comparison function for the particular sort type.
 *  @param[in] type The sort type
 *  @return A std::function object with the correct comparison for
 *          the sort type.
 */
CompareP get_comp(SortType type);

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_OPERATIONS_SORT_HH
