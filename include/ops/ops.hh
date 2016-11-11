#include "global.hh"
#include <memory>
#include "file/file.hh"

#warning Make redundant
#include "cppfileapi.hh"
namespace PIOL { namespace File {

/*! A structure to hold a reference to a single coordinate and
 * the corresponding trace number
 */
struct CoordElem
{
    geom_t val;     //!< The value
    size_t num;     //!< The trace number
};

/*! Get the min and the max of a set of parameters passed. This is a parallel operation. It is
 *  the collective min and max across all processes (which also must all call this file).
 *  \param[in, out] piol The PIOL object
 *  \param[in] offset The starting trace number (local).
 *  \param[in] lnt The local number of traces to process.
 *  \param[in] coord
 *  \param[out] minmax An array of structures containing the minimum item.x,  maximum item.x, minimum item.y, maximum item.y
 *  and their respective trace numbers.
*/
extern void getMinMax(ExSeisPIOL * piol, size_t offset, size_t lnt, const coord_t * coord, CoordElem * minmax);

//TODO: Write a test for this function
/*! Get the min and the max of a set of parameters passed. This is a parallel operation. It is
 *  the collective min and max across all processes (which also must all call this file).
 *  \param[in, out] piol The PIOL object
 *  \param[in] offset The starting trace number (local).
 *  \param[in] lnt The local number of traces to process.
 *  \param[in] item The coordinate item of interest.
 *  \param[in] prm An array of trace parameter structures
 *  \param[out] minmax An array of structures containing the minimum item.x,  maximum item.x, minimum item.y, maximum item.y
 *  and their respective trace numbers.
 */
extern void getMinMax(ExSeisPIOL * piol, size_t offset, size_t lnt, File::Coord item, const TraceParam * prm, CoordElem * minmax);

enum class SortType : size_t
{
    SrcRcv,
    Line,
    OffsetLine,
    CmpSrc
};

extern std::vector<size_t> Sort(ExSeisPIOL * piol, File::Interface * src, size_t offset, size_t lnt);
extern std::vector<size_t> Sort(ExSeisPIOL * piol, SortType type, File::Interface * src, size_t offset, size_t lnt);
}}
