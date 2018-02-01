////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date November 2016
/// @brief The Sort Operation
/// @details The algorithm used is a nearest neighbour approach where at each
///          iteration the lowest valued metadata entries are moved to adjacent
///          processes with a lower rank and a sort is performed. After the sort
///          the highest entries are moved again to the process with a higher
///          rank. If each process has the same traces it started off with, the
///          sort is complete.
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLOPSSORT_INCLUDE_GUARD
#define PIOLOPSSORT_INCLUDE_GUARD

#include "anc/piol.hh"
#include "global.hh"
#include "share/api.hh"
#include "share/param.hh"

namespace PIOL {
namespace File {

/************************************ Core ************************************/
/*! Function to sort the metadata in a Param struct. The returned vector is the
 *  location where the nth parameter is located in the sorted list.
 *  Implementation note: the Param vector is used internally to allow
 *  random-access iterator support.
 *  @param[in] piol The PIOL object.
 *  @param[in,out] prm The parameter structure to sort
 *  @param[in] comp The Param function to use for less-than comparisons between
 *                  objects in the vector. It assumes each Param structure has
 *                  exactly one entry.
 *  @param[in] FileOrder Do we wish to have the sort in the sorted input order
 *                       (true) or sorted order (false)
 *  @return Return the correct order of traces from those which are smallest
 *          with respect to the comp function.
 */
std::vector<size_t> sort(
  ExSeisPIOL* piol, Param* prm, CompareP comp, bool FileOrder = true);

/*! Check that the file obeys the expected ordering.
 *  @param[in] src The input file.
 *  @param[in] dec The decomposition: a pair which contains the offset (first)
 *                 and the number of traces for the local process.
 *  @return Return true if the local ordering is correct.
 */
bool checkOrder(ReadInterface* src, std::pair<size_t, size_t> dec);

/********************************** Non-Core **********************************/
/*! Perform a sort on the given parameter structure.
 *  @param[in] piol The PIOL object
 *  @param[in] type The sort type
 *  @param[in,out] prm The trace parameter structure.
 *  @return Return a vector which is a list of the ordered trace numbers. i.e
 *          the 0th member is the position of the 0th trace post-sort.
 */
std::vector<size_t> sort(ExSeisPIOL* piol, SortType type, Param* prm);

/*! Check that the file obeys the expected ordering.
 *  @param[in] src The input file.
 *  @param[in] dec The decomposition: a pair which contains the offset (first)
 *                 and the number of traces for the local process.
 *  @param[in] type The sort type
 *  @return Return true if the local ordering is correct.
 */
bool checkOrder(
  ReadInterface* src, std::pair<size_t, size_t> dec, SortType type);

/*! Return the comparison function for the particular sort type.
 *  @param[in] type The sort type
 *  @return A std::function object with the correct comparison for
 *          the sort type.
 */
CompareP getComp(SortType type);

}  // namespace File
}  // namespace PIOL

#endif
