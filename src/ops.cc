#include "global.hh"
#include <algorithm>
#include <iterator>
#include "file/file.hh"

namespace PIOL { namespace File {
/*
auto extractCrd(Coord crd)
{
    switch (crd)
    {
        case Src :
            return [] (const TraceParam & v) -> TraceParam & { return v.src };
        case Rcv :
            return [] (const TraceParam & v) -> TraceParam & { return v.rcv };
        default :
        case CMP :
            return [] (const TraceParam & v) -> TraceParam & { return v.cmp };
    }
}


std::vector<coord_t> & getCoordPoint(Coord crd, size_t sz, const TraceParam * prm)
{
    std::vector<coord_t> vec(sz);
    auto elem = extractCrd(crd);
    for (size_t i = 0; i < sz; i++)
        std::transform(std::begin(prm), std::end(&prm[sz-1]), vec.begin(), elem);

    return vec;
}
*/

/*//TODO: This is temporarily hosting the communication. Move this to the communication layer
void getExtent(Piol piol, size_t sz, Coord crd, const TraceParam * prm,  std::pair<coord_t, size_t> * extent)
{
    auto crdvec = getCoordPoint(crd, sz, prm);
    std::vector<geom_t> xvec(sz), yvec(sz);
    std::transform(crdvec.begin(), crdvec.end(), xvec.begin(), [] (const coord_t & c) -> coord_t & { return c.first; });
    std::transform(crdvec.begin(), crdvec.end(), yvec.begin(), [] (const coord_t & c) -> coord_t & { return c.second; });

    crdvec.resize(0);

    auto xres = std::minmax_element(xvec.begin(), xvec.end());
    geom_t xval = xres;
    size_t xnum = std::distance(xvec.begin(), xres);
    xvec.resize(0);

    auto yres = std::minmax_element(yvec.begin(), yvec.end());
    geom_t yval = yres;
    size_t ynum = std::distance(yvec.begin(), yres);
    yvec.resize(0);

    std::vector< gather(yval);
    gather(yval);


    return ;
}*/

geom_t xmin(Piol piol, size_t offset, size_t sz, const coord_t * val)
{
    std::vector<geom_t> xvec(sz);
    std::transform(val, &val[sz], xvec.begin(), [] (const coord_t & c) -> geom_t { return c.first; });

    //Local
    auto xres = std::min_element(xvec.begin(), xvec.end());
    geom_t xval = *xres;
    auto xnt = piol->comm->gather(offset + std::distance(xvec.begin(), xres));
    xvec.resize(0);

    //Global
    xvec = piol->comm->gather(xval);
    xres = std::min_element(xvec.begin(), xvec.end());

    return *xres;
}

/*template <typename T>
using RedOp = std::function<Iterator::T(Iterator::T, Iterator::T)>;

template <typename T, size_t Item>
T reducOp(RedOp op, Iterator::T start, Iterator T end, std::pair<T, T> * val)
{
    std::vector<T> vec(sz);
    std::transform(val, &val[sz], vec.begin(), [] (const std::pair<T, T> & c) -> geom_t { return std::get<Item>(c); });

    //Local
    auto xres = op(vec.begin(), vec.end());
    geom_t val = *res;
    auto nt = piol->comm->gather(offset + std::distance(vec.begin(), res));
    vec.resize(0);

    //Global
    vec = piol->comm->gather(val);
    res = op(vec.begin(), vec.end());
    return *res;
}*/

geom_t xmin(Piol piol, size_t sz, const coord_t * val)
{
//    return reducOp([](Iterator::geom , Iterator::T start, Iterator T end, std::pair<T, T> * val);
    return xmin(piol, 0, sz, val);
}
}}


