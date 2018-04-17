////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date November 2016
/// @brief
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_OPERATIONS_MINMAX_H
#define EXSEISDAT_PIOL_OPERATIONS_MINMAX_H

#include "ExSeisDat/utils/typedefs.h"

/*! A structure to hold a reference to a single coordinate and the corresponding
 *  trace number.
 *  Defined as struct for C-compatibility.
 */
struct PIOL_CoordElem {
    /// The value
    exseis_geom_t val;

    /// The trace number
    size_t num;
};

#ifdef __cplusplus
namespace exseis {
namespace PIOL {
/// @copydoc PIOL_CoordElem
typedef PIOL_CoordElem CoordElem;
}  // namespace PIOL
}  // namespace exseis
#endif  // __cplusplus


#ifdef __cplusplus

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/Param.h"
#include "ExSeisDat/utils/typedefs.h"

#include <algorithm>
#include <functional>
#include <iterator>

namespace exseis {
namespace PIOL {

using namespace exseis::utils::typedefs;

/// Return the value associated with a particular parameter
template<typename T>
using MinMaxFunc = std::function<geom_t(const T&)>;

/************************************ Core ************************************/
/*! Get the min and max for a parameter. Use a second parameter to decide
 *  between equal cases.
 *  @tparam T The type of the input array
 *  @param[in] piol The PIOL object
 *  @param[in] offset The offset for the local process
 *  @param[in] sz The number of sets of parameters for the local process
 *  @param[in] coord The array of parameters for the local process
 *  @param[in] elem1 The function for extracting the first parameter from
 *                   \c coord
 *  @param[in] elem2 The function for extracting the second parameter from
 *                   \c coord
 *  @return Return a vector of length 2 with the Coordinates as elements
 */
template<typename T>
std::vector<CoordElem> getCoordMinMax(
  ExSeisPIOL* piol,
  size_t offset,
  size_t sz,
  const T* coord,
  MinMaxFunc<T> elem1,
  MinMaxFunc<T> elem2)
{
    auto min = [elem1, elem2](const T& a, const T& b) -> bool {
        if (elem1(a) < elem1(b)) return true;
        if (elem1(a) == elem1(b) && elem2(a) < elem2(b)) return true;
        return false;
    };

    T temp;
    if (!sz || !coord) coord = &temp;

    auto p                      = std::minmax_element(coord, coord + sz, min);
    std::vector<geom_t> lminmax = {elem1(*p.first), elem1(*p.second)};
    std::vector<size_t> ltrace  = {offset + std::distance(coord, p.first),
                                  offset + std::distance(coord, p.second)};

    auto tminmax = piol->comm->gather(lminmax);
    auto ttrace  = piol->comm->gather(ltrace);
    auto tsz     = piol->comm->gather(std::vector<size_t>{sz});

    // Remove non-participants
    for (llint i = tsz.size() - 1U; i >= 0; i--)
        if (!tsz[i]) {
            tminmax.erase(tminmax.begin() + 2U * i + 1U);
            tminmax.erase(tminmax.begin() + 2U * i);
            ttrace.erase(ttrace.begin() + 2U * i + 1U);
            ttrace.erase(ttrace.begin() + 2U * i);
        }

    // Global
    auto s = std::minmax_element(tminmax.begin(), tminmax.end());
    std::vector<CoordElem> minmax;
    if (s.first != s.second) {
        minmax = {
          CoordElem{*s.first, ttrace[std::distance(tminmax.begin(), s.first)]},
          CoordElem{*s.second,
                    ttrace[std::distance(tminmax.begin(), s.second)]}};
    }
    else {
        minmax = {CoordElem{0, std::numeric_limits<size_t>::max()},
                  CoordElem{0, std::numeric_limits<size_t>::max()}};
    }

    return minmax;
}

/*! Get the min and max for both parameters.
 * @tparam T The type of the input array
 * @param[in] piol The PIOL object
 * @param[in] offset The offset for the local process
 * @param[in] sz The number of sets of parameters for the local process
 * @param[in] coord The array of parameters for the local process
 * @param[in] xlam The function for extracting the first parameter from \c coord
 * @param[in] ylam The function for extracting the second parameter from
 *                 \c coord
 * @param[in] minmax An array of length 4 with the CoordElem structure as
 *                   elements.  min for xlam, max for xlam, min for ylam, max
 *                   for ylam.
 */
template<typename T>
void getMinMax(
  ExSeisPIOL* piol,
  size_t offset,
  size_t sz,
  const T* coord,
  MinMaxFunc<T> xlam,
  MinMaxFunc<T> ylam,
  CoordElem* minmax)
{
    auto x = getCoordMinMax<T>(piol, offset, sz, coord, xlam, ylam);
    auto y = getCoordMinMax<T>(piol, offset, sz, coord, ylam, xlam);

    if (minmax) {
        std::copy(x.begin(), x.end(), minmax);
        std::copy(y.begin(), y.end(), minmax + x.size());
    }
}

/********************************** Non-Core **********************************/

// TODO: Write a test for this function
/*! Get the min and the max of a set of parameters passed. This is a parallel
 *  operation. It is the collective min and max across all processes (which also
 *  must all call this file).
 *  @param[in, out] piol The PIOL object
 *  @param[in] offset The starting trace number (local).
 *  @param[in] sz The local number of traces to process.
 *  @param[in] m1 The first coordinate item of interest.
 *  @param[in] m2 The second coordinate item of interest.
 *  @param[in] prm An array of trace parameter structures
 *  @param[out] minmax An array of structures containing the minimum item.x,
 *                     maximum item.x, minimum item.y, maximum item.y
 *  and their respective trace numbers.
 */
void getMinMax(
  ExSeisPIOL* piol,
  size_t offset,
  size_t sz,
  Meta m1,
  Meta m2,
  const Param* prm,
  CoordElem* minmax);

}  // namespace PIOL
}  // namespace exseis

#endif  // __cplusplus

#endif  // EXSEISDAT_PIOL_OPERATIONS_MINMAX_H
