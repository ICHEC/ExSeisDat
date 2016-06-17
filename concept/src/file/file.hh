#ifndef PIOLFILEFILE_INCLUDE_GUARD
#define PIOLFILEFILE_INCLUDE_GUARD
#include <vector>
#include <array>
#include <cassert>
#include <utility>
#include <string>
#include <iostream>
#include <memory>
#include <mpi.h>
#include "object/object.hh"
#include "comm/comm.hh"
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

struct Header
{
    std::string note;
    real inc;
    size_t ns;
    size_t nt;
};

class Interface
{
    protected:
    std::string note;
    real inc;
    size_t ns;
    size_t nt;

    size_t rank;
    size_t numRank;

    std::shared_ptr<Comms::Interface> comm;
    std::unique_ptr<Obj::Interface> obj;
    public :

    typedef std::pair<real, real> CoordData;
    typedef std::pair<BlockMd, BlockMd> CoordPair;
    typedef std::array<CoordData, static_cast<size_t>(Coord::Len)> CoordArray;

    Interface(std::shared_ptr<Comms::Interface> Comm) : comm(Comm)
    {

    }
    Header getHeader()
    {
        Header header;
        header.note = getNote();
        header.ns = getNs();
        header.nt = getNt();
        header.inc = getInc();
        return header;
    }
    virtual std::string getNote()
    {
        return note;
    }
    virtual size_t getNs()
    {
        return ns;
    }
    virtual size_t getNt()
    {
        return nt;
    }
    virtual real getInc()
    {
        return inc;
    }

    void setHeader(Header & header)
    {
        setNote(header.note);
        setNs(header.ns, false);
        setNt(header.nt, false);
        setInc(header.inc);
        checkFileSize();
    }

    virtual void setNote(std::string Note)
    {
        note = Note;
    }
    virtual void setNs(size_t Ns, bool updateFile = true)
    {
        ns = Ns;
        if (updateFile)
            checkFileSize();
    }
    virtual void setNt(size_t Nt, bool updateFile = true)
    {
        nt = Nt;
        if (updateFile)
            checkFileSize();
    }
    virtual void setInc(real Inc)
    {
        inc = Inc;
    }
    virtual void getCoord(size_t, CoordPair, std::vector<CoordData> &) = 0;
    virtual void getCoord(size_t, Coord, std::vector<CoordData> &) = 0;
    virtual void getAllCoords(size_t, std::vector<CoordArray> &) = 0;

    virtual void setCoord() = 0;

//Contiguous Block
    virtual void getTraces(size_t, std::vector<real> &) = 0;
//Random access pattern. Good candidate for collective.
    virtual void getTraces(std::vector<size_t> &, std::vector<real> &) = 0;
    virtual void setTraces() = 0;

    void checkFileSize()
    {
        if (obj->getFileSz() != obj->getSize(getNt(), getNs()))
        {
            obj->setFileSz(getNt(), getNs());
        }
    }
    void setFile(Header & header, std::vector<CoordArray> & coord, std::vector<real> & data)
    {
        assert((coord.size() == data.size() / header.ns) && (header.nt*header.ns == data.size()));

    }
    void getFile(Header & header, std::vector<CoordArray> & coord, std::vector<real> & data)
    {


    }
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
