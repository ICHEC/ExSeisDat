/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date March 2017
 *   \detals For small functions outside the library that don't make sense anywhere else.
*//*******************************************************************************************/
#include <vector>
#include <algorithm>
#include <numeric>
namespace PIOL
{
/*! Get the sorted index associated with a given list (support function)
 *  \param[in] sz The length of the list
 *  \param[in] list The array of numbers
 *  \return A vector containing the numbering of list in a sorted order
 */
std::vector<size_t> getSortIndex(size_t sz, const size_t * list)
{
    std::vector<size_t> index(sz);
    std::iota(index.begin(), index.end(), 0);
    std::sort(index.begin(), index.end(), [list] (size_t s1, size_t s2) { return list[s1] < list[s2]; });
    return index;
}
}
