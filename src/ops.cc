#include "global.hh"
#include <algorithm>
#include <iterator>
#include "ops/ops.hh"
#include "file/file.hh"

#warning remove
#include <iostream>

namespace PIOL { namespace File {
/*geom_t xmin(Piol piol, size_t offset, size_t sz, const coord_t * coord)
{
    std::vector<geom_t> xvec(sz);
    std::transform(coord, &coord[sz], xvec.begin(), [] (const coord_t & c) -> geom_t { return c.x; });

    //Local
    auto xres = std::min_element(xvec.begin(), xvec.end());
    geom_t xcoord = *xres;
    auto xnt = piol->comm->gather(std::vector<size_t>{offset + std::distance(xvec.begin(), xres)});
    xvec.resize(0);

    //Global
    xvec = piol->comm->gather(std::vector<geom_t>{xcoord});
    xres = std::min_element(xvec.begin(), xvec.end());

    return *xres;
}*/

template <bool GetY>
std::vector<CoordElem> getCoordMinMax(Piol piol, size_t offset, size_t sz, const coord_t * coord)
{
    auto minY = [] (const File::coord_t & a, const File::coord_t & b) -> bool
    {
        if (a.y < b.y)
            return true;
        if (a.y == b.y && a.x < b.x)
            return true;
        return false;
    };
    auto minX = [] (const File::coord_t & a, const File::coord_t & b) -> bool
    {
        if (a.x < b.x)
            return true;
        if (a.x == b.x && a.y < b.y)
            return true;
        return false;
    };
    auto p = std::minmax_element(coord, coord + sz, (GetY ? minY : minX));

    auto lminmax = (GetY ? std::vector<geom_t>{ p.first->y, p.second->y } :
                           std::vector<geom_t>{ p.first->x, p.second->x });
    std::vector<size_t> ltrace = {offset + std::distance(coord, p.first),
                                  offset + std::distance(coord, p.second)};
    auto tminmax = piol->comm->gather(lminmax);
    auto ttrace = piol->comm->gather(ltrace);
    //Global
    auto s = std::minmax_element(tminmax.begin(), tminmax.end());

    std::vector<CoordElem> minmax = {{*s.first, ttrace[std::distance(tminmax.begin(), s.first)]},
                                     {*s.second, ttrace[std::distance(tminmax.begin(), s.second)]}};
    return minmax;

}

void getMinMax(Piol piol, size_t offset, size_t sz, const coord_t * coord, CoordElem * minmax)
{
    //TODO: Create and use a subcommunicator to avoid the deadlock the next conditions will cause
    if (!sz || !coord)
    {
        piol->record("", Log::Layer::Ops, Log::Status::Warning,
                     "getCoordMinMax() was called but a process (" + std::to_string(piol->comm->getRank()) +") has no traces.",
                     Log::Verb::Extended);
        return;
    }
    if (!minmax)
    {
        piol->record("", Log::Layer::Ops, Log::Status::Warning,
                     "getCoordMinMax() was called but a process (" + std::to_string(piol->comm->getRank()) +") does not have minmax allocated.",
                     Log::Verb::Extended);
        return;
    }

    auto x = getCoordMinMax<false>(piol, offset, sz, coord);
    auto y = getCoordMinMax<true>(piol, offset, sz, coord);

    std::copy(x.begin(), x.end(), minmax);
    std::copy(y.begin(), y.end(), minmax + x.size());
}
}}
