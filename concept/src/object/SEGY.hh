#ifndef PIOLOBJSEGY_GUARD
#define PIOLOBJSEGY_GUARD
//Concept test. Naive implementations
#include <vector>
#include <cassert>
#include <string>
#include <iostream>
#include "block/block.hh"

namespace PIOL { namespace Obj { namespace SEGY {
enum class SZ : size_t
{
    BinHdr = 400,
    TextHdr = 3200,
    HO = BinHdr+TextHdr,
    Md = 240
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
constexpr size_t getMDSz()
{
    return static_cast<size_t>(SZ::Md);
}
constexpr size_t getHOSz()
{
    return static_cast<size_t>(SZ::HO);
}

template <typename T>
inline size_t getDOSz(size_t ns)
{
    return getMDSz()+ns*sizeof(T);
}
template <typename T>
inline size_t getDOLoc(size_t i, size_t ns)
{
    return getHOSz() + i*getDOSz<T>(ns);
}

template <typename T>
inline size_t getDODFLoc(size_t i, size_t ns)
{
    return getDOLoc<T>(i, ns) + getMDSz();
}

}}}
#endif
