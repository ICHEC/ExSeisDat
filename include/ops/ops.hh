#include "global.hh"
#include <memory>
#include "file/file.hh"
namespace PIOL { namespace File {
struct CoordElem
{
    geom_t val;
    size_t num;
};

extern void getMinMax(Piol piol, size_t offset, size_t sz, const coord_t * coord, CoordElem * minmax);
//TODO: Untested:
extern void getMinMax(Piol piol, size_t offset, size_t sz, File::Coord item, const TraceParam * prm, CoordElem * minmax);
}}
