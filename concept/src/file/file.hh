#ifndef PIOLFILEFILE_INCLUDE_GUARD
#define PIOLFILEFILE_INCLUDE_GUARD
#include <vector>
#include <cassert>
#include <utility>
#include <string>
#include <iostream>
#include <mpi.h>
#include "object/object.hh"
namespace PIOL { 
typedef float real;
namespace File {
enum class Md : size_t
{
    Increment,
    NumSample,
    NumTraces,
    Type
};

enum class BlockMd : size_t
{
    xSrc,
    ySrc,
    xRcv,
    yRcv,
    xCDP,
    yCDP,
    iLin,
    xLin,
    ERROR
};

enum class Coord : size_t
{
    Src,
    Rcv,
    Cmp,
    Lin
};

constexpr 
std::pair<BlockMd, BlockMd> getCoordPair(Coord pair)
{
    switch (pair)
    {
        case Coord::Src :
            return std::make_pair<BlockMd, BlockMd>(BlockMd::xSrc, BlockMd::ySrc);
        case Coord::Rcv :
            return std::make_pair<BlockMd, BlockMd>(BlockMd::xRcv, BlockMd::yRcv);
        case Coord::Cmp :
            return std::make_pair<BlockMd, BlockMd>(BlockMd::xCDP, BlockMd::yCDP);
        case Coord::Lin :
            return std::make_pair<BlockMd, BlockMd>(BlockMd::iLin, BlockMd::xLin);
        default : 
            return std::make_pair<BlockMd, BlockMd>(BlockMd::ERROR, BlockMd::ERROR);
    }
}
class Interface
{
    protected:
    std::string note;
    real inc;
    size_t ns;
    size_t nt;
    public :

    typedef std::vector<std::pair<real, real> > CoordData;
    typedef std::pair<BlockMd, BlockMd> CoordPair;

    size_t getNs()
    {
        return ns;
    }
    size_t getNt()
    {
        return nt;
    }
    real getInc()
    {
        return inc;
    }
    virtual void getCoord(size_t, CoordPair Items, CoordData & data) = 0;
    virtual void getCoord(size_t start, Coord coord, CoordData & data) = 0;
    virtual void setCoord() = 0;
    virtual void getTraces() = 0;
    virtual void setTraces() = 0;
};
#include "SEGY.hh"
}}
#endif
