#ifndef PIOLFILE_INCLUDE_GUARD
#define PIOLFILE_INCLUDE_GUARD
#include <vector>
#include <mpi.h>
#include <cassert>
#include <execinfo.h>
#include "global.hh"
#include "object/object.hh"
#include "comm/comm.hh"
namespace PIOL { namespace File {
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
//    Water,
    Len
};

enum class Grid : size_t
{
    Lin,
    Len
};

struct Header
{
    std::string text;
    real inc;
    size_t ns;
    size_t nt;
};

template <typename T>
using Pair = std::pair<T, T>;

typedef Pair<coreal> CoordData;
typedef Pair<llint> GridData;
typedef Pair<BlockMd> MetaPair;

template <typename T, typename P>
using MetaArray = std::array<std::pair<T, T>, static_cast<size_t>(P::Len)>;

typedef MetaArray<coreal, Coord> CoordArray;
typedef MetaArray<llint, Grid> GridArray;

//typedef std::array<CoordData, static_cast<size_t>(Coord::Len)> CoordArray;
//typedef std::array<GridData, static_cast<size_t>(Grid::Len)> GridArray;

class Interface
{
    protected:
    std::string text;
    real inc;
    size_t ns;
    size_t nt;

    size_t rank;
    size_t numRank;

    std::shared_ptr<Comms::Interface> comm;
    std::unique_ptr<Obj::Interface> obj;

    bool defHOUpdate;
    void checkFileSize(void);

    public :

    Interface(std::shared_ptr<Comms::Interface> Comm);
    ~Interface(void);
    void readHeader(Header &);

    virtual std::string readText(void);
    virtual size_t readNs(void);
    virtual size_t readNt(void);
    virtual real readInc(void);

    virtual void writeText(std::string Text);
    virtual void writeNs(size_t Ns);
    virtual void writeNt(size_t Nt);
    virtual void writeInc(real Inc);

    virtual void writeHeader(Header header) = 0;

    virtual void readTraces(size_t offset, std::vector<real> & data) = 0;
//Random access pattern. Good candidate for collective.
    virtual void readTraces(std::vector<size_t> & offset, std::vector<real> & data) = 0;
    virtual void writeTraces(size_t offset, std::vector<real> & data) = 0;
    inline void readTraces(std::vector<real> & data)
    {
        readTraces(0U, data);
    }
    inline void writeTraces(std::vector<real> & data)
    {
        writeTraces(0U, data);
    }

    virtual void readCoord(size_t offset, MetaPair items, std::vector<CoordData> & data) = 0;
    virtual void readCoord(size_t offset, std::vector<CoordArray> & data) = 0;
    virtual void readCoord(size_t offset, Coord coord, std::vector<CoordData> & data) = 0;
    virtual void writeCoord(size_t offset, Coord coord, std::vector<CoordData> & data) = 0;
    virtual void writeCoord(size_t offset, std::vector<CoordArray> & data) = 0;
    inline void readCoord(MetaPair items, std::vector<CoordData> & data)
    {
        readCoord(0U, items, data);
    }
    inline void readCoord(std::vector<CoordArray> & data)
    {
        readCoord(0U, data);
    }
    inline void readCoord(Coord coord, std::vector<CoordData> & data)
    {
        readCoord(0U, coord, data);
    }
    inline void writeCoord(Coord coord, std::vector<CoordData> & data)
    {
        writeCoord(0U, coord, data);
    }
    inline void writeCoord(std::vector<CoordArray> & data)
    {
        writeCoord(0U, data);
    }

    virtual void readGrid(size_t offset, MetaPair items, std::vector<GridData> & data) = 0;
    virtual void readGrid(size_t offset, std::vector<GridArray> & data) = 0;
    virtual void readGrid(size_t offset, Grid grid, std::vector<GridData> & data) = 0;
    virtual void writeGrid(size_t offset, Grid grid, std::vector<GridData> & data) = 0;
    virtual void writeGrid(size_t offset, std::vector<GridArray> & data) = 0;

    virtual void readTraceHeader(size_t offset, std::vector<GridArray> &, std::vector<CoordArray> &) = 0;
    virtual void writeTraceHeader(size_t offset, std::vector<GridArray> &, std::vector<CoordArray> &) = 0;

    void writeFile(Header & header, std::vector<CoordArray> & coord, std::vector<real> & data);
    void readFile(Header & header, std::vector<CoordArray> & coord, std::vector<real> & data);
};
#ifndef __ICC
constexpr
#else
inline
#endif
MetaPair getPair(Coord pair)
{
    switch (pair)
    {
        case Coord::Src :
            return std::make_pair(BlockMd::xSrc, BlockMd::ySrc);
        case Coord::Rcv :
            return std::make_pair(BlockMd::xRcv, BlockMd::yRcv);
        case Coord::Cmp :
            return std::make_pair(BlockMd::xCDP, BlockMd::yCDP);
//        case Coord::Water :
//            return std::make_pair(BlockMd::wdSrc, BlockMd::wdGrp);
        default :
            assert(0);
            return std::make_pair(BlockMd::ERROR, BlockMd::ERROR);
    }
}
#ifndef __ICC
constexpr
#else
inline
#endif
MetaPair getPair(Grid pair)
{
    switch (pair)
    {
        case Grid::Lin :
            return std::make_pair(BlockMd::iLin, BlockMd::xLin);
        default :
            return std::make_pair(BlockMd::ERROR, BlockMd::ERROR);
    }
}
}}
#endif
