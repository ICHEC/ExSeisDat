/*******************************************************************************************//*!
 *   @file
 *   @author Cathal O Broin - cathal@ichec.ie - first commit
 *   @copyright TBD. Do not distribute
 *   @date March 2017
 *   @details For small functions outside the library that don't make sense anywhere else.
*//*******************************************************************************************/
#ifndef PIOLSHAREMISC_INCLUDE_GUARD
#define PIOLSHAREMISC_INCLUDE_GUARD

#include <cstddef>
#include <vector>

namespace PIOL {

/*! Get the sorted index associated with a given list (support function)
 *  @param[in] sz The length of the list
 *  @param[in] list The array of numbers
 *  @return A vector containing the numbering of list in a sorted order
 */
std::vector<size_t> getSortIndex(size_t sz, const size_t* list);

}  // namespace PIOL

#endif
