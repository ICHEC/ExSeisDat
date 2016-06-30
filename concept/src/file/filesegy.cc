#include "file/filesegy.hh"

#include <mpi.h>
#include <iconv.h> //For EBCDIC conversion
#include <arpa/inet.h>

#include <vector>
#include <array>
#include <cassert>
#include <cmath>
#include <string>
#include <iostream>

#include "global.hh"
#include "object/object.hh"
#include "file/iconv.hh"

namespace PIOL { namespace File {
const double MICRO = 1e-6;

template <typename T = short>
T getHostShort(const uchar * src)
{
    return (T(src[0]) << 8) | T(src[1]);
}

template <typename T = int>
T getHostInt(const uchar * src)
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

template <class T> inline
int roundToInt(T scale, T data)
{
    if ((typeid(coreal) == typeid(T)) || (typeid(real) == typeid(T)))
        return std::lround(data / scale);
    else
    {
        //The implicit down-conversion has the potential of losing information.
        //Not really sure what I can do about that. SEG-Y limitation on datasize.

        return static_cast<int>(data);
    }
}

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
T getMd(TrHdr val, const uchar * src)
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
constexpr size_t getTextSz()
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

coreal convertScale(int scale)
{
    auto rs = coreal(std::abs(scale));
    return (scale > 0 ? rs : coreal(1)) / (scale < 0 ? rs : coreal(1));
}

template <typename T>
int getTraceScale(const uchar * dos)
{
    return T(1);
}

//template <typename T> typename std::enable_if<std::is_floating_point<T>::value, bool>::type
template <>
int getTraceScale<coreal>(const uchar * dos)
{
    int scale = getMd<coreal>(TrHdr::ScaleCoord, dos);
    return convertScale(scale);
}


int calcAppropScale(coreal val)
{
    coreal intpart;
    coreal fracpart = std::modf(val, &intpart);
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
        coreal scalp;
        int scal = 10000;
        coreal rm = std::modf(coreal(scal * fracpart), &scalp);
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

int calcTraceScale(const CoordArray & data)
{
    int scale = 10000;
    for (size_t j = 0; j < static_cast<size_t>(Coord::Len); j++)
    {
        if (data[j].first != coreal(0))
            scale = std::min(calcAppropScale(data[j].first), scale);
        if (data[j].second == coreal(0))
            scale = std::min(calcAppropScale(data[j].second), scale);
        //scale = std::min(calcAppropScale(data[j].second), std::min(calcAppropScale(data[j].first), scale));
    }
    return scale;
}

template <typename P, typename T>
T setTraceScale(uchar * dos, const MetaArray<T, P> & data)
{
    return T(1);
}

template <>
coreal setTraceScale<Coord, coreal>(uchar * dos, const MetaArray<coreal, Coord> & data)
{
    int scale = calcTraceScale(data);
    setMd(TrHdr::ScaleCoord, dos, scale);
    return convertScale(scale);
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
//template <typename T=int>
void setMd(Hdr val, uchar * dst, int src)
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

template <typename T>
void extractPair(std::vector<uchar> & dos, const MetaPair & pair, std::vector<std::pair<T, T> > & data)
{
    size_t mds = Obj::SEGSz::getMDSz();
    TrHdr md1 = getItem(pair.first);
    TrHdr md2 = getItem(pair.second);

    for (size_t i = 0; i < data.size(); i++)
    {
        T scale = getTraceScale<T>(&dos[i*mds]);
        T val1 = static_cast<T>(getMd(md1, &dos[i*mds]));
        T val2 = static_cast<T>(getMd(md2, &dos[i*mds]));
        data[i] = std::make_pair(scale*val1, scale*val2);
    }
}

template <typename P, typename T>
void extractArray(const uchar * dos, MetaArray<T, P> & data)
{
    T scale = getTraceScale<T>(dos);
    for (size_t j = 0; j < static_cast<size_t>(P::Len); j++)
    {
        auto p = getPair(static_cast<P>(j));
        T val1 = static_cast<T>(getMd(getItem(p.first), dos));
        T val2 = static_cast<T>(getMd(getItem(p.second), dos));
        data[j] = std::make_pair(scale*val1, scale*val2);
    }
}

template <typename P, typename T>
void insertArray(uchar * dos, const MetaArray<T, P> & data)
{
    T scale = setTraceScale<P, T>(dos, data);
    for (size_t j = 0; j < static_cast<size_t>(P::Len); j++)
    {
        auto p = getPair(static_cast<P>(j));
        setMd(getItem(p.first), dos, roundToInt(scale, data[j].first));
        setMd(getItem(p.second), dos, roundToInt(scale, data[j].second));
    }
}

template <typename P, typename T>
void extractPair(std::vector<uchar> & dos, std::vector<MetaArray<T, P> > & data)
{
    size_t mds = Obj::SEGSz::getMDSz();
    for (size_t i = 0; i < data.size(); i++)
    {
        extractArray<P, T>(&dos[i*mds], data[i]);
    }
}

template <typename P, typename T>
void insertPair(std::vector<uchar> & dos, const std::vector<MetaArray<T, P> > & data)
{
    size_t mds = Obj::SEGSz::getMDSz();
    for (size_t i = 0; i < data.size(); i++)
    {
        insertArray<P, T>(&dos[i*mds], data[i]);
    }
}

void extractHeader(const std::vector<uchar> & dos, std::vector<TraceHeader> & data)
{
    size_t mds = Obj::SEGSz::getMDSz();

    for (size_t i = 0; i < data.size(); i++)
    {
        extractArray<Coord, coreal>(&dos[i*mds], data[i].coords);
        extractArray<Grid, llint>(&dos[i*mds], data[i].grids);
    }

//    MPI_Barrier(MPI_COMM_WORLD);
//    if (!comm->getRank()) std::cout << "Extraction complete\n";


}

void insertHeader(std::vector<uchar> & dos, const std::vector<TraceHeader> & data)
{
    size_t mds = Obj::SEGSz::getMDSz();
    for (size_t i = 0; i < data.size(); i++)
    {
        insertArray<Coord, coreal>(&dos[i*mds], data[i].coords);
        insertArray<Grid, llint>(&dos[i*mds], data[i].grids);
    }
}

void print(std::vector<char> str)
{
    for (char p : str)
        std::cout << p;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////SEGY File Interface/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void SEGY::readDataTraces(std::vector<size_t> & offsets, std::vector<uchar> & data)
{
    size_t ns = readNs();
    size_t dsz = Obj::SEGSz::getDOSz<float>(ns);
    data.resize(offsets.size() * dsz);

    for (size_t i = 0; i < offsets.size(); i++)
        obj->readDO(offsets[i], 1, &data.data()[i*dsz], ns);
}

void SEGY::writeDataTraces(size_t offset, std::vector<uchar> & data)
{
    size_t ns = readNs();
    size_t dsz = Obj::SEGSz::getDOSz<float>(ns);
    assert(!(data.size() % dsz));
    size_t num = data.size() / dsz;

    obj->writeDO(offset, num, data.data(), ns);
}

void SEGY::writeTraceHeader(size_t offset, std::vector<TraceHeader> & thead)
{
    size_t num = thead.size();
    size_t mds = Obj::SEGSz::getMDSz();
    std::vector<uchar> dos(num * mds);
    insertHeader(dos, thead);
    obj->writeDOMD(offset, num, dos.data(), ns);
}

void SEGY::readTraceHeader(size_t offset, std::vector<TraceHeader> & thead)
{
    std::vector<uchar> dos = readTraceHeaders(offset, thead.size());
//    if (!comm->getRank()) std::cout << "traceHeader  read: " << thead.size() << std::endl;
    extractHeader(dos, thead);
}

void SEGY::readTraceHeader(size_t offset, std::vector<GridArray> & grid, std::vector<CoordArray> & coord)
{
    std::vector<uchar> dos = readTraceHeaders(offset, std::max(grid.size(), coord.size()));
    extractPair<Coord>(dos, coord);
    extractPair<Grid>(dos, grid);
}

void SEGY::writeTraceHeader(size_t offset, std::vector<GridArray> & grid, std::vector<CoordArray> & coord)
{
    size_t num = std::max(grid.size(), coord.size());
    size_t mds = Obj::SEGSz::getMDSz();
    std::vector<uchar> dos(num * mds);
    insertPair<Coord, coreal>(dos, coord);
    insertPair<Grid, llint>(dos, grid);
    obj->writeDOMD(offset, num, dos.data(), ns);
}

void SEGY::writeTraceHeaders(size_t offset, std::vector<uchar> & data)
{
    std::cerr << "writeTraceHeaders not implemented\n";
    assert(0);
}

std::vector<uchar> SEGY::readTraceHeaders(size_t offset, size_t num)
{
    size_t mds = Obj::SEGSz::getMDSz();
    std::vector<uchar> dos(num * mds);
    obj->readDOMD(offset, num, dos.data(), ns);
    return dos;
}

void SEGY::parseHO(std::vector<uchar> & buf)
{
    IConvert ic;
    std::vector<char> ttext(getTextSz());
    for (size_t i = 0; i < getTextSz(); i++)
        ttext[i] = buf[i];
    ttext = ic.getAscii(ttext);

    text.resize(getTextSz());
    for (size_t i = 0; i < getTextSz(); i++)
        text[i] = ttext[i];
    format = static_cast<Format>(getMd(Hdr::Type, buf.data()));
    ns = getMd(Hdr::NumSample, buf.data());
    inc = coreal(getMd(Hdr::Increment, buf.data())) * MICRO;

    size_t fsz = obj->getFileSz();
    nt = (fsz - Obj::SEGSz::getHOSz()) / Obj::SEGSz::getDOSz<float>(ns);

    assert(ns != 0);
    assert(fsz - nt*Obj::SEGSz::getDOSz<float>(ns) - Obj::SEGSz::getHOSz() == 0);
    assert(fsz - obj->getSize(nt, ns) == 0);
}

std::vector<uchar> SEGY::makeHeader()
{
    std::vector<uchar> header(Obj::SEGSz::getHOSz());
    std::string texts = readText();

    for (size_t i = 0; i < texts.size(); i++)
        header[i] = texts[i];

    assert(readNs() < (1 << (sizeof(short)*8+1))); //TODO: Make sure each data amount is compatible
    setMd(Hdr::NumSample, header.data(), static_cast<short>(readNs()));
    setMd(Hdr::Type, header.data(), static_cast<short>(Format::IEEE));

    int incr = static_cast<int>(std::lround(readInc() / MICRO));
    setMd(Hdr::Increment, header.data(), incr);

    return header;
}

SEGY::SEGY(std::shared_ptr<Comms::Interface> Comm, std::string name, Bt bType) : File::Interface::Interface(Comm)
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
        text = "";
    }
}
void SEGY::writeHeader(Header & header)
{
    writeText(header.text);
    writeNs(header.ns);
    writeNt(header.nt);
    writeInc(header.inc);


    if (!comm->getRank())
    {
        std::cout << "Write Header\n";
        std::cout << "text: " << readText() << std::endl;
        std::cout << "nt: " << readNt() << std::endl;
        std::cout << "ns: " << readNs() << std::endl;
        std::cout << "inc: "<< readInc() << std::endl;
    }

    std::vector<uchar> buf = makeHeader();

    obj->setFileSz(readNt(), readNs());
    obj->writeHO(buf.data());

    //Whether this was true or false, considering
    //we just wrote the HO, it's not deferred
    defHOUpdate = false;
}

void SEGY::readCoord(size_t offset, MetaPair items, std::vector<CoordData> & data)
{
    std::vector<uchar> dos = readTraceHeaders(offset, data.size());
    extractPair(dos, items, data);
}

//TODO: what if Vector accesses outside end of file?
void SEGY::readCoord(size_t offset, Coord coord, std::vector<CoordData> & data)
{
    readCoord(offset, getPair(coord), data);
}

void SEGY::writeCoord(size_t offset, Coord coord, std::vector<CoordData> & data)
{
    std::cerr << "writeCoord not implemented yet" << std::endl;
    assert(0);
}
void SEGY::readCoord(size_t offset, std::vector<CoordArray> & data)
{
    std::vector<uchar> dos = readTraceHeaders(offset, data.size());
    extractPair<Coord>(dos, data);
}

//TODO: bounds check
//TODO: Nt Check
//TODO: This overwrites any previous writes such as grid data
void SEGY::writeCoord(size_t offset, std::vector<CoordArray> & data)
{
    size_t num = data.size();
    size_t mds = Obj::SEGSz::getMDSz();
    std::vector<uchar> dos(num * mds);
    insertPair<Coord>(dos, data);
    obj->writeDOMD(offset, num, dos.data(), ns);
}

//TODO: This overwrites any previous writes such as coord data
void SEGY::writeGrid(size_t offset, std::vector<GridArray> & data)
{
    size_t num = data.size();
    size_t mds = Obj::SEGSz::getMDSz();
    std::vector<uchar> dos(num * mds);
    insertPair<Grid>(dos, data);
    obj->writeDOMD(offset, num, dos.data(), ns);
}

void SEGY::readGrid(size_t offset, MetaPair items, std::vector<GridData> & data)
{
    std::vector<uchar> dos = readTraceHeaders(offset, data.size());
    extractPair(dos, items, data);
}

void SEGY::readGrid(size_t offset, std::vector<GridArray> & data)
{
    std::vector<uchar> dos = readTraceHeaders(offset, data.size());
    extractPair<Grid>(dos, data);
}

void SEGY::readGrid(size_t offset, Grid grid, std::vector<GridData> & data)
{
    std::vector<uchar> dos = readTraceHeaders(offset, data.size());
    extractPair(dos, getPair(grid), data);
}
void SEGY::writeGrid(size_t offset, Grid grid, std::vector<GridData> & data)
{
    std::cerr << "writeGrid not implemented" << std::endl;
    assert(0);
}

//TODO: what if Vector accesses outside end of file?
//TODO: convert traces to little endian
void SEGY::readTraces(size_t offset, std::vector<real> & data)
{
    size_t num = data.size() / ns;
    obj->readDODF(offset, num, data.data(), ns);

//        for (real & d : data)
//            makeLittleEndian(d);
}

//TODO: what if Vector accesses outside end of file?
void SEGY::readTraces(std::vector<size_t> & offset, std::vector<real> & data)
{
    for (size_t i = 0; i < data.size(); i++)
    {
        obj->readDODF(offset[i], 1U, &data[i*ns], ns);
    }
}

//TODO: what if Vector accesses outside end of file?
//TODO: convert traces to big endian
void SEGY::writeTraces(size_t offset, std::vector<real> & src)
{
    std::vector<real> data(src.size());
    for (size_t i = 0; i < src.size(); i++)
        data[i] = makeBigEndian(src[i]);

    size_t num = data.size() / ns;
    obj->writeDODF(offset, num, data.data(), ns);
}
}}
