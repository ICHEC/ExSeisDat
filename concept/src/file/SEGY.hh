#ifndef PIOLFILESEGY_INCLUDE_GUARD
#define PIOLFILESEGY_INCLUDE_GUARD
#include <vector>
#include <array>
#include <cassert>
#include <cmath>
#include <string>
#include <iostream>
#include <mpi.h>
#include <iconv.h> //For EBCDIC conversion
#include <arpa/inet.h>
#include "global.hh"
#include "object/objsegy.hh"
#include "file/file.hh"
#include "file/iconv.hh"

namespace PIOL { namespace File { namespace SEGY {

const double MICRO = 1e-6;

template <typename T = short>
T getHostShort(uchar * src)
{
    return (T(src[0]) << 8) | T(src[1]); 
}

template <typename T = int>
T getHostInt(uchar * src)
{
    return (static_cast<T>(src[0]) << 24) |
           (static_cast<T>(src[1]) << 16) |
           (static_cast<T>(src[2]) << 8) |
            static_cast<T>(src[3]); 
}

template <typename T = short>
void getBigShort(uchar * dst, T src)
{
    dst[0] = src >> 8 & 0xFF;
    dst[1] = src & 0xFF;
}

template <typename T = int>
void getBigInt(uchar * dst, T src)
{
    dst[0] = src >> 24 & 0xFF;
    dst[1] = src >> 16 & 0xFF;
    dst[2] = src >> 8 & 0xFF;
    dst[3] = src & 0xFF;
}
void makeLittleEndian(float & d)
{
    union { float d; uint32_t i; } src;
    src.d = d;
    src.i = ntohl(src.i);
    d = src.i;
}

float makeBigEndian(float d)
{
    union { float d; uint32_t i; } src;
    src.d = d;
    src.i = htonl(src.i);
    return src.d;
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
T getMd(TrHdr val, uchar * src)
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

            return getHostInt(&src[static_cast<size_t>(val)-1U]);
        case TrHdr::ScaleCoord :
            return getHostShort(&src[static_cast<size_t>(val)-1U]);
        default :
            std::cerr << "Unknown Item\n";
            return T(0);
        break;
    }
}
constexpr size_t getNoteSz()
{
    return 3200U;
}

template <class T=int>
void setMd(TrHdr val, uchar * dst, T src)
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
            getBigInt(&dst[static_cast<size_t>(val)-1U], src);
            break;
        case TrHdr::ScaleCoord :
            getBigShort(&dst[static_cast<size_t>(val)-1U], src);
            break;
        default :
            std::cerr << "setMd: Unknown Item\n";
        break;
    }
}

real getTraceScale(uchar * dos)
{
    int scale = getMd<real>(TrHdr::ScaleCoord, dos);
    auto rs = real(std::abs(scale));
    return (scale > 0 ? rs : 1.0) / (scale < 0 ? rs : 1.0);
}

template <class T=int>
T getMd(Hdr val, uchar * buf)
{
    switch (val)
    {
        case Hdr::Increment :
        case Hdr::Type :
        case Hdr::NumSample :
        return getHostShort(&buf[size_t(val)-1]);
        default :
        return 0;
        break;
    }
}
template <typename T=int>
void setMd(Hdr val, uchar * dst, T src)
{
    switch (val)
    {
        case Hdr::Increment :
        case Hdr::Type :
        case Hdr::NumSample :
        getBigShort(&dst[static_cast<size_t>(val)-1U], src);
        default :
        break;
    }
}

void print(std::vector<char> str)
{
    for (char p : str)
        std::cout << p;
}

class Interface : public PIOL::File::Interface
{
    typedef PIOL::Block::Type Bt;

    Format format;
    size_t scale;
    
    void parseHO(std::vector<uchar> & buf)
    {
        iconvert ic;
        std::vector<char> tnote(getNoteSz());
        for (size_t i = 0; i < getNoteSz(); i++)
            tnote[i] = buf[i];
        tnote = ic.getAscii(tnote);
    
        note.resize(getNoteSz());
        for (size_t i = 0; i < getNoteSz(); i++)
            note[i] = tnote[i];
        format = static_cast<Format>(getMd(Hdr::Type, buf.data()));
        ns = getMd(Hdr::NumSample, buf.data());
        inc = real(getMd(Hdr::Increment, buf.data())) * MICRO;

        size_t fsz = obj->getFileSz();
        nt = (fsz - Obj::SEGSz::getHOSz()) / Obj::SEGSz::getDOSz<float>(ns);

        assert(ns != 0);
        assert(fsz - nt*Obj::SEGSz::getDOSz<float>(ns) - Obj::SEGSz::getHOSz() == 0); 
        assert(fsz - obj->getSize(nt, ns) == 0);
    }

    std::vector<uchar> makeHeader()
    {
        std::vector<uchar> header(Obj::SEGSz::getHOSz());
        std::string notes = readNote();
        std::cout << "Note size " << notes.size() << " HO " << Obj::SEGSz::getHOSz() << std::endl;

        for (size_t i = 0; i < notes.size(); i++)
            header[i] = notes[i];

        std::cout << "Obj::SEGSz::getHOSz() = "<< Obj::SEGSz::getHOSz() << std::endl; 
        std::cout << "Ns = " << readNs() << std::endl;
        assert(readNs() <  (1 << (sizeof(short)*8+1)));

        setMd(Hdr::NumSample, header.data(), short(readNs()));
        //setMd(Hdr::Type, header.data(), static_cast<int>(format));
        setMd(Hdr::Type, header.data(), static_cast<short>(Format::IEEE));

        int incr = static_cast<int>(std::lround(readInc() / MICRO));
        setMd(Hdr::Increment, header.data(), incr);

        return header;
    }

    public :

    Interface(std::shared_ptr<Comms::Interface> Comm, std::string name, Bt bType) : File::Interface::Interface(Comm)
    {
        obj = std::unique_ptr<Obj::Interface>(new Obj::SEGY(comm, name, bType));

        if (obj->getFileSz() != 0U)
        {
            std::vector<uchar> buf(Obj::SEGSz::getHOSz());
            obj->readHO(buf.data());
            parseHO(buf);
        }
        else
        {
            ns = 0U;
            nt = 0U;
            inc = 0.0;
            note = "";
        }
    }
    void writeHeader(Header header)
    {
        writeNote(header.note);
        writeNs(header.ns);
        writeNt(header.nt);
        writeInc(header.inc);

        std::vector<uchar> buf = makeHeader();

        obj->setFileSz(readNt(), readNs());

        std::cout << "CALL writeHO\n";
        for (size_t i = 3200U; i < buf.size(); i++)
            if (buf[i])
            std::cout << "header" << i+1 << " " << int(buf[i]) << std::endl;

        obj->writeHO(buf.data());

        //Whether this was true or false, considering
        //we just wrote the HO, it's not deferred
        defHOUpdate = false;
    }
    void readCoord(size_t offset, CoordPair items, std::vector<CoordData> & data)
    {
        TrHdr md1 = getItem(items.first);
        TrHdr md2 = getItem(items.second);

        size_t num = data.size();
        size_t mds = Obj::SEGSz::getMDSz();
        std::vector<uchar> dos(num * mds);
        obj->readDOMD(offset, num, dos.data(), ns);

        for (size_t i = 0; i < num; i++)
        {
            int val1 = getMd(md1, &dos[i*mds]);
            int val2 = getMd(md2, &dos[i*mds]);
            real scale = getTraceScale(&dos[i*mds]);

            data[i] = std::make_pair(real(scale*val1), real(scale*val2));
        }
    }
    
    void readCoord(size_t offset, std::vector<CoordArray> & data)
    {
        size_t num = data.size();
        size_t mds = Obj::SEGSz::getMDSz();
        std::vector<uchar> dos(num * mds);
        obj->readDOMD(offset, num, dos.data(), ns);

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

//TODO: what if Vector accesses outside end of file?
    void readCoord(size_t offset, Coord coord, std::vector<CoordData> & data)
    {
        readCoord(offset, getCoordPair(coord), data);
    }

    void writeCoord(size_t offset, Coord coord, std::vector<CoordData> & data)
    {

    }

int getScale(real val)
{
    real intpart;
    real fracpart = std::modf(val, &intpart);
    //TODO: Account for overflow
    int intp = std::lround(intpart);
    if (fracpart < 1e-5)
    {
        for (int scal = 10; scal > 100000; scal *= 10)
        {
            int t = intp / scal;
            if ((scal * t) != intp)
            {
                return scal/10;
            }
        }
    }
    else
    {
        real scalp;
        int scal = 10000;
        real rm = std::modf(real(scal * fracpart), &scalp);
        int piece = std::lround(scalp);
        for (int i = 10; i != 10*scal; scal *= 10)
        {
            if (piece % i)
            {
                return - i;
            }
        }
    }
    return 1;
}

//TODO: bounds check
//TODO: Nt Check
    void writeCoord(size_t offset, std::vector<CoordArray> & data)
    {
        size_t num = data.size();
        size_t mds = Obj::SEGSz::getMDSz();

        std::vector<uchar> dos(num * mds);

        for (size_t i = 0; i < num; i++)
        {
            int scale = 10000;
            for (size_t j = 0; j < static_cast<size_t>(Coord::Len); j++)
            {
                scale = std::min(getScale(data[i][j].second), std::min(getScale(data[i][j].first), scale));
            }
            //TODO: set scale
            setMd(TrHdr::ScaleCoord, &dos[i*mds], scale);
            auto rs = real(std::abs(scale));
            rs = (scale > 0 ? rs : 1.0) / (scale < 0 ? rs : 1.0);

            for (size_t j = 0; j < static_cast<size_t>(Coord::Len); j++)
            {
                auto p = getCoordPair(static_cast<Coord>(j));
                setMd(getItem(p.first), &dos[i*mds], std::lround(data[i][j].first / rs));
                setMd(getItem(p.second), &dos[i*mds], std::lround(data[i][j].second / rs));
            }
        }
        obj->writeDOMD(offset, num, dos.data(), ns);
    }

//TODO: what if Vector accesses outside end of file?
//TODO: convert traces to little endian
    void readTraces(size_t offset, std::vector<real> & data)
    {
        size_t num = data.size() / ns;
        obj->readDODF(offset, num, data.data(), ns);

//        for (real & d : data)
//            makeLittleEndian(d);
    }

//TODO: what if Vector accesses outside end of file?
    void readTraces(std::vector<size_t> & offset, std::vector<real> & data)
    {
        for (size_t i = 0; i < data.size(); i++)
        {
            obj->readDODF(offset[i], 1U, &data[i*ns], ns);
        }
    }

//TODO: what if Vector accesses outside end of file?
//TODO: convert traces to big endian
    void writeTraces(size_t offset, std::vector<real> & src)
    {
        std::vector<real> data(src.size());
        for (size_t i = 0; i < src.size(); i++)
            data[i] = makeBigEndian(src[i]);

        size_t num = data.size() / ns;
        obj->writeDODF(offset, num, data.data(), ns);
    }
};
}}}
#endif
