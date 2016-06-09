//Concept test. Naive implementations
#include <vector>
#include <cassert>
#include <string>
#include <iostream>
#include "block/block.hh"

namespace PIOL { namespace Obj { namespace SEGY {
enum SEGYSz : size_t
{
    BinHdrSz = 400,
    TextHdrSz = 3200,
    HdrSz = BinHdrSz+TextHdrSz,
    THdrSz = 240
};

/*void readHO(Block::BlockLayer & block, std::vector<char> & buf)
{
    buf.resize(HdrSz);
    block.getData(0, buf.data(), buf.size());
}*/
/*
    *data = new T * [sz];
    *data[0] = new T [sz*cnt];
    for (size_t i = 0; i < sz; i++)
        *data[i] = &(*data)[0][i*cnt];
*/
template <typename T>
inline size_t getDOSz(size_t ns)
{
    return THdrSz+ns*sizeof(T);
}
template <typename T>
inline size_t getDOLoc(size_t i, size_t ns)
{
    return HdrSz + i*getDOSz<T>(ns);
}

template <typename T>
inline size_t getDODFLoc(size_t i, size_t ns)
{
    return getDOLoc<T>(i, ns) + THdrSz;
}

template <typename T>
void readObj(Block::BlockLayer & block, size_t start, size_t sz, T * data)
{
    block.readData(start, data, sz);
}
template <typename T>
void writeObj(Block::BlockLayer & block, size_t start, size_t sz, T * data)
{
    block.writeData(start, data, sz);
}

/////////////////////////////////////////////////////////////
/////////////////////READ////////////////////////////////////
/////////////////////////////////////////////////////////////

void readHO(Block::BlockLayer & block, char * data)
{
    readObj<char>(block, 0, HdrSz, data);
}

void readDO(Block::BlockLayer & block, size_t start, size_t sz, char * dos, size_t ns)
{
    readObj<char>(block, start, sz*getDOSz<float>(ns), dos);
}

void readDODF(Block::BlockLayer & block, size_t start, size_t sz, char ** data, size_t ns)
{
    for (size_t i = 0; i < sz; i++)
        readObj<char>(block, getDODFLoc<float>(start+i, ns), ns, data[i]);
}

void readDOMD(Block::BlockLayer & block, size_t start, size_t sz, char ** data, size_t ns)
{
    for (size_t i = 0; i < sz; i++)
        readObj<char>(block, getDOLoc<float>(start+i, ns), THdrSz, data[i]);
}

/////////////////////////////////////////////////////////////
/////////////////////WRITE///////////////////////////////////
/////////////////////////////////////////////////////////////

void writeHO(Block::BlockLayer & block, char * data)
{
    writeObj<char>(block, 0, HdrSz, data);
}

void writeDO(Block::BlockLayer & block, size_t start, size_t sz, char * dos, size_t ns)
{
    writeObj<char>(block, start, sz*getDOSz<float>(ns), dos);
}

void writeDODF(Block::BlockLayer & block, size_t start, size_t sz, char ** data, size_t ns)
{
    for (size_t i = 0; i < sz; i++)
        writeObj<char>(block, getDODFLoc<float>(start+i, ns), ns, data[i]);
}

void writeDOMD(Block::BlockLayer & block, size_t start, size_t sz, char ** data, size_t ns)
{
    for (size_t i = 0; i < sz; i++)
        writeObj<char>(block, getDOLoc<float>(start+i, ns), THdrSz, data[i]);
}
}}}

