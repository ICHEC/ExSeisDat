#ifndef PIOLFILESEGY_INCLUDE_GUARD
#define PIOLFILESEGY_INCLUDE_GUARD
#include <vector>
#include <cassert>
#include <string>
#include <iostream>
#include <mpi.h>
#include "object/object.hh"
#include "file/file.hh"
namespace PIOL { namespace File { namespace SEGY {

const double MICRO = 1e-6;

template <typename T = short>
T makeShort(char * src)
{
    return (src[0] << 8) | src[1]; 
}
template <typename T = int>
T makeInt(char * src)
{
    return (src[0] << 24) | (src[0] << 16) | (src[0] << 8) | src[1]; 
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
//check about line number
    ScaleCoord = 71, //2 byte int
    xSrc = 73,  //4 byte int
    ySrc = 77,  //4 byte int
    xGrp = 81,  //4 byte int
    yGrp = 85,  //4 byte int
    xCDP = 181, //4 byte int
    yCDP = 185, //4 byte int
};
template <class T=int>
T getMd(TrHdr val, char * buf)
{
    switch (val)
    {
        case TrHdr::xSrc :
        case TrHdr::ySrc :
        case TrHdr::xGrp :
        case TrHdr::yGrp :
        case TrHdr::xCDP :
        case TrHdr::yCDP :
        return T((long long int)(makeShort(&buf[size_t(TrHdr::ScaleCoord)-1]))
               * (long long int)(makeInt(&buf[size_t(val)-1])));
//        case TrHdr::ScaleCoord :
//        return makeShort(&buf[size_t(val)-1]);
        default :
        return 0;
        break;
    }
}
template <class T=int>
T getMd(Hdr val, char * buf)
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

namespace Obj = PIOL::Obj::SEGY;

class FileSEGY : public PIOL::File::FileLayer
{
    typedef PIOL::Block::Type Bt;

    Format format;
    Obj::ObjSEGY obj;
    size_t scale;
    
    void getTraceHeader()
    {
        std::vector<char> buf(Obj::getHOSz());
        obj.readHO(buf.data());
        format = static_cast<Format>(getMd(Hdr::Type, buf.data()));
        ns = getMd(Hdr::NumSample, buf.data());
        inc = real(getMd(Hdr::Increment, buf.data())) * MICRO;

        size_t fsz = obj.getFileSz();

        nt = (fsz - Obj::getHOSz()) / Obj::getDOSz<float>(ns);

        assert(ns != 0);
        assert(fsz - nt*Obj::getDOSz<float>(ns) - Obj::getHOSz() == 0); 
        assert(fsz - obj.getSize(nt, ns) == 0);
    }
    
    public :

    FileSEGY(MPI_Comm comm, std::string name, Bt bType) : obj(comm, name, bType)
    {
        getTraceHeader();
    }

    void getCoord()
    {
    }
    void setCoord()
    {
    }
    void getTraces()
    {
    }
    void setTraces()
    {
    }
};
}}}
#endif
