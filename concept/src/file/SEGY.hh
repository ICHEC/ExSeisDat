#ifndef PIOLFILESEGY_INCLUDE_GUARD
#define PIOLFILESEGY_INCLUDE_GUARD
#include <vector>
#include <array>
#include <cassert>
#include <cmath>
#include <string>
#include <iostream>
#include <mpi.h>
#include "object/objsegy.hh"
#include "file/file.hh"
namespace PIOL { namespace File { namespace SEGY {

const double MICRO = 1e-6;

template <typename T = short>
T makeShort(unsigned char * src)
{
    return (T(src[0]) << 8) | T(src[1]); 
}
template <typename T = int>
T makeInt(unsigned char * src)
{
    return (static_cast<T>(src[0]) << 24) |
           (static_cast<T>(src[1]) << 16) |
           (static_cast<T>(src[2]) << 8) |
            static_cast<T>(src[3]); 
}
template <typename T = int>
T makeHostEndian(T src)
{
    return (0x000F & src) << 24 |
           (0x00F0 & src) << 8 |
           (0x0F00 & src) >> 8 |
           (0xF000 & src) >> 24; 
}

enum Format : short
{
    IBM = 1,    //IBM format, big endian
    TC4,    //Two's complement, 4 byte
    TC2,    //Two's complement, 2 byte
    FPG,    //Fixed-point gain (obsolete)
    IEEE,
    NA1,
    NA2,
    TC1    //Two's complement, 1 byte
};



enum class Hdr : size_t
{
    Increment = 3217, //Short
    NumSample = 3221, //Short
    Type = 3225 //Short, Trace data type. AKA format in SEGY terminology
};
enum class TrHdr : size_t
{
    SeqNum = 1, //4 byte int
    SeqFNum = 5, //4 byte int
    ScaleCoord = 71, //2 byte int
    xSrc = 73,  //4 byte int
    ySrc = 77,  //4 byte int
    xGrp = 81,  //4 byte int
    yGrp = 85,  //4 byte int
    xCDP = 181, //4 byte int
    yCDP = 185, //4 byte int
    iLin = 189, //4 byte int
    xLin = 193, //4 byte int
    ERROR = 0
};

#ifndef __ICC
constexpr
#endif
TrHdr getItem(BlockMd val)
{
    switch (val)
    {
        case BlockMd::xSrc :
            return TrHdr::xSrc;
        case BlockMd::ySrc :
            return TrHdr::ySrc;
        case BlockMd::xRcv :
            return TrHdr::xGrp;
        case BlockMd::yRcv :
            return TrHdr::yGrp;
        case BlockMd::xCDP :
            return TrHdr::xCDP;
        case BlockMd::yCDP :
            return TrHdr::yCDP;
        case BlockMd::iLin :
            return TrHdr::iLin;
        case BlockMd::xLin :
            return TrHdr::xLin;
        default : 
            return TrHdr::ERROR;
    }
}

template <class T=int>
T getMd(TrHdr val, unsigned char * buf)
{
    switch (val)
    {
        case TrHdr::SeqNum :
        case TrHdr::SeqFNum :
        case TrHdr::xSrc :
        case TrHdr::ySrc :
        case TrHdr::xGrp :
        case TrHdr::yGrp :
        case TrHdr::xCDP :
        case TrHdr::yCDP :
        case TrHdr::iLin :
        case TrHdr::xLin :

            return makeInt(&buf[static_cast<size_t>(val)-1U]);
        case TrHdr::ScaleCoord :
            return makeShort(&buf[static_cast<size_t>(val)-1U]);
        default :
            std::cerr << "Unknown Item\n";
            return T(0);
        break;
    }
}
real getTraceScale(unsigned char * dos)
{
    int scale = getMd<real>(TrHdr::ScaleCoord, dos);
    auto rs = real(std::abs(scale));
    return (scale > 0 ? rs : 1.0) / (scale < 0 ? rs : 1.0);
}

template <class T=int>
T getMd(Hdr val, unsigned char * buf)
{
    switch (val)
    {
        case Hdr::Increment :
        case Hdr::Type :
        case Hdr::NumSample :
        return makeShort(&buf[size_t(val)-1]);
        default :
        return 0;
        break;
    }
}

///namespace Obj = PIOL::Obj::SEGY;

class Interface : public PIOL::File::Interface
{
    typedef PIOL::Block::Type Bt;

    Format format;
    size_t scale;
    
    void parseHO(std::vector<unsigned char> & buf)
    {
        format = static_cast<Format>(getMd(Hdr::Type, buf.data()));
        ns = getMd(Hdr::NumSample, buf.data());
        inc = real(getMd(Hdr::Increment, buf.data())) * MICRO;

        size_t fsz = obj->getFileSz();
        nt = (fsz - Obj::SEGY::getHOSz()) / Obj::SEGY::getDOSz<float>(ns);

        assert(ns != 0);
        assert(fsz - nt*Obj::SEGY::getDOSz<float>(ns) - Obj::SEGY::getHOSz() == 0); 
        assert(fsz - obj->getSize(nt, ns) == 0);
    }
    
    public :

    Interface(Comms::Interface & Comm, std::string name, Bt bType) : File::Interface::Interface(Comm)
    {
        obj = std::unique_ptr<Obj::Interface>(new Obj::SEGY::Interface(comm, name, bType));

        std::vector<unsigned char> buf(Obj::SEGY::getHOSz());
        obj->readHO(buf.data());
        parseHO(buf);
    }

    void getCoord(size_t start, CoordPair items, std::vector<CoordData> & data)
    {
        TrHdr md1 = getItem(items.first);
        TrHdr md2 = getItem(items.second);

        size_t num = data.size();
        size_t mds = Obj::SEGY::getMDSz();
        std::vector<unsigned char> dos(num * mds);
        obj->readDOMD(start, num, dos.data(), ns);

        for (size_t i = 0; i < num; i++)
        {
            int val1 = getMd(md1, &dos[i*mds]);
            int val2 = getMd(md2, &dos[i*mds]);
            real scale = getTraceScale(&dos[i*mds]);

            data[i] = std::make_pair(real(scale*val1), real(scale*val2));
        }
    }
    
    void getAllCoords(size_t start, std::vector<CoordArray> & data)
    {
        size_t num = data.size();
        size_t mds = Obj::SEGY::getMDSz();
        std::vector<unsigned char> dos(num * mds);
        obj->readDOMD(start, num, dos.data(), ns);

        for (size_t i = 0; i < num; i++)
        {
            real scale = getTraceScale(&dos[i*mds]);
            for (size_t j = 0; j < static_cast<size_t>(Coord::Len); j++)
            {
                auto p = getCoordPair(static_cast<Coord>(j));
                int val1 = getMd(getItem(p.first), &dos[i*mds]);
                int val2 = getMd(getItem(p.second), &dos[i*mds]);
                data[i][j] = std::make_pair(real(scale*val1), real(scale*val2));
            }
        }
    }

    void getCoord(size_t offset, Coord coord, std::vector<CoordData> & data)
    {
        getCoord(offset, getCoordPair(coord), data);
    }

    void setCoord()
    {
    }

    void getTraces(size_t offset, std::vector<real> & data)
    {
        size_t num = data.size() / ns;
        obj->readDODF(offset, num, data.data(), ns);
    }

    void getTraces(std::vector<size_t> & offset, std::vector<real> & data)
    {
        for (size_t i = 0; i < data.size(); i++)
        {
            obj->readDODF(offset[i], 1U, &data[i*ns], ns);
        }
    }

    void setTraces()
    {
    }
};
}}}
#endif
