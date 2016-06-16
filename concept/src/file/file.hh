#ifndef PIOLFILEFILE_INCLUDE_GUARD
#define PIOLFILEFILE_INCLUDE_GUARD
#include <vector>
#include <array>
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

    Len,
    ERROR
};

enum class Coord : size_t
{
    Src,
    Rcv,
    Cmp,
    Lin,
    Len
};

class Interface
{
    protected:
    std::string note;
    real inc;
    size_t ns;
    size_t nt;
    public :

    typedef std::pair<real, real> CoordData;
    typedef std::pair<BlockMd, BlockMd> CoordPair;
    typedef std::array<CoordData, static_cast<size_t>(Coord::Len)> CoordArray;

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

    virtual void getCoord(size_t, CoordPair, std::vector<CoordData> &) = 0;
    virtual void getCoord(size_t, Coord, std::vector<CoordData> &) = 0;
    virtual void getAllCoords(size_t start, std::vector<CoordArray> & data) = 0;

    virtual void setCoord() = 0;

//Contiguous Block
    virtual void getTraces(size_t, std::vector<real> &) = 0;
//Random access pattern. Good candidate for collective.
    virtual void getTraces(std::vector<size_t> &, std::vector<real> &) = 0;

    virtual void setTraces() = 0;
};

#ifndef __ICC
constexpr 
#endif
Interface::CoordPair getCoordPair(Coord pair)
{
    switch (pair)
    {
        case Coord::Src :
            return std::make_pair(BlockMd::xSrc, BlockMd::ySrc);
        case Coord::Rcv :
            return std::make_pair(BlockMd::xRcv, BlockMd::yRcv);
        case Coord::Cmp :
            return std::make_pair(BlockMd::xCDP, BlockMd::yCDP);
        case Coord::Lin :
            return std::make_pair(BlockMd::iLin, BlockMd::xLin);
        default : 
            return std::make_pair(BlockMd::ERROR, BlockMd::ERROR);
    }
}

#include "SEGY.hh"
}}
#endif
