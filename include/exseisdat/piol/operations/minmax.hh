////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date November 2016
/// @brief
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_OPERATIONS_MINMAX_HH
#define EXSEISDAT_PIOL_OPERATIONS_MINMAX_HH

#include "exseisdat/piol/operations/minmax.h"

#include "exseisdat/piol/configuration/ExSeisPIOL.hh"
#include "exseisdat/piol/metadata/Trace_metadata.hh"
#include "exseisdat/utils/types/typedefs.hh"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iterator>

namespace exseis {
namespace piol {
inline namespace operations {

/// @brief Import PIOL_CoordElem from C API.
/// @copydoc PIOL_CoordElem
typedef PIOL_CoordElem CoordElem;

/// @brief Return the value associated with a particular parameter
template<typename T>
using MinMaxFunc = std::function<exseis::utils::Floating_point(const T&)>;

/************************************ Core ************************************/
/*! @brief Get the min and max for a parameter. Use a second parameter to decide
 *         between equal cases.
 *
 *  @tparam     T       The type of the input array
 *
 *  @param[in]  piol    The PIOL object
 *  @param[in]  offset  The offset for the local process
 *  @param[in]  sz      The number of sets of parameters for the local process
 *  @param[in]  coord   The array of parameters for the local process
 *  @param[in]  elem1   The function for extracting the first parameter from
 *                      \c coord
 *  @param[in]  elem2   The function for extracting the second parameter from
 *                      \c coord
 *
 *  @return Return a vector of length 2 with the Coordinates as elements
 */
template<typename T>
std::vector<CoordElem> get_coord_min_max(
    ExSeisPIOL* piol,
    size_t offset,
    size_t sz,
    const T* coord,
    MinMaxFunc<T> elem1,
    MinMaxFunc<T> elem2)
{
    auto min = [elem1, elem2](const T& a, const T& b) -> bool {
        if (elem1(a) < elem1(b)) {
            return true;
        }
        if (elem1(a) > elem1(b)) {
            return false;
        }

        // elem1(a) == elem1(b)

        return elem2(a) < elem2(b);
    };

    T temp;
    if (!sz || !coord) {
        coord = &temp;
    }

    auto p = std::minmax_element(coord, coord + sz, min);

    std::vector<exseis::utils::Floating_point> lminmax = {elem1(*p.first),
                                                          elem1(*p.second)};

    // Check conversions are ok.
    assert(std::distance(coord, p.first) >= 0);
    assert(std::distance(coord, p.second) >= 0);

    std::vector<size_t> ltrace = {
        offset + static_cast<size_t>(std::distance(coord, p.first)),
        offset + static_cast<size_t>(std::distance(coord, p.second))};

    auto tminmax = piol->comm->gather(lminmax);
    auto ttrace  = piol->comm->gather(ltrace);
    auto tsz     = piol->comm->gather(std::vector<size_t>{sz});

    // Remove non-participants
    for (size_t ii = tsz.size(); ii != 0; ii--) {
        const auto i = ii - 1;

        if (tsz[i] == 0) {
            using Difference = decltype(tminmax)::difference_type;

            tminmax.erase(std::next(tminmax.begin(), Difference(2 * i + 1)));
            tminmax.erase(std::next(tminmax.begin(), Difference(2 * i)));
            ttrace.erase(std::next(ttrace.begin(), Difference(2 * i + 1)));
            ttrace.erase(std::next(ttrace.begin(), Difference(2 * i)));
        }
    }

    // Global
    auto s = std::minmax_element(tminmax.begin(), tminmax.end());
    std::vector<CoordElem> minmax;
    if (s.first != s.second) {
        const auto d_first  = std::distance(tminmax.begin(), s.first);
        const auto d_second = std::distance(tminmax.begin(), s.second);

        assert(d_first >= 0);
        assert(d_second >= 0);

        minmax = {CoordElem{*s.first, ttrace[static_cast<size_t>(d_first)]},
                  CoordElem{*s.second, ttrace[static_cast<size_t>(d_second)]}};
    }
    else {
        minmax = {CoordElem{0, std::numeric_limits<size_t>::max()},
                  CoordElem{0, std::numeric_limits<size_t>::max()}};
    }

    return minmax;
}

/*! @brief Get the min and max for both parameters.
 *
 * @tparam      T The type of the input array
 *
 * @param[in]   piol    The PIOL object
 * @param[in]   offset  The offset for the local process
 * @param[in]   sz      The number of sets of parameters for the local process
 * @param[in]   coord   The array of parameters for the local process
 * @param[in]   xlam    The function for extracting the first parameter from
 *                      \c coord
 * @param[in]   ylam    The function for extracting the second parameter from
 *                      \c coord
 * @param[in]   minmax  An array of length 4 with the CoordElem structure as
 *                      elements.  min for xlam, max for xlam, min for ylam, max
 *                      for ylam.
 */
template<typename T>
void get_min_max(
    ExSeisPIOL* piol,
    size_t offset,
    size_t sz,
    const T* coord,
    MinMaxFunc<T> xlam,
    MinMaxFunc<T> ylam,
    CoordElem* minmax)
{
    auto x = get_coord_min_max<T>(piol, offset, sz, coord, xlam, ylam);
    auto y = get_coord_min_max<T>(piol, offset, sz, coord, ylam, xlam);

    if (minmax) {
        std::copy(x.begin(), x.end(), minmax);
        std::copy(y.begin(), y.end(), minmax + x.size());
    }
}

/********************************** Non-Core **********************************/

// TODO: Write a test for this function
/*! @brief Get the min and the max of a set of parameters passed. This is a
 *         parallel operation. It is the collective min and max across all
 *         processes (which also must all call this file).
 *
 *  @param[in,out]  piol    The PIOL object
 *  @param[in]      offset  The starting trace number (local).
 *  @param[in]      sz      The local number of traces to process.
 *  @param[in]      m1      The first coordinate item of interest.
 *  @param[in]      m2      The second coordinate item of interest.
 *  @param[in]      prm     An array of trace parameter structures
 *  @param[out]     minmax  An array of structures containing the minimum
 *                          item.x, maximum item.x, minimum item.y, maximum
 *                          item.y and their respective trace numbers.
 */
void get_min_max(
    ExSeisPIOL* piol,
    size_t offset,
    size_t sz,
    Trace_metadata_key m1,
    Trace_metadata_key m2,
    const Trace_metadata& prm,
    CoordElem* minmax);

}  // namespace operations
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_OPERATIONS_MINMAX_HH
