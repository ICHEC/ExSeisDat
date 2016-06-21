#ifndef PIOLOBJSEGY_INCLUDE_GUARD
#define PIOLOBJSEGY_INCLUDE_GUARD
#include <memory>
#include "comm/mpi.hh"
#include "object/object.hh"
#include "block/block.hh"

namespace PIOL { namespace Obj {
class SEGY : public PIOL::Obj::Interface
{
    public : 
    SEGY(std::shared_ptr<Comms::Interface> comm, std::string name, Bt bType);
    //Assumes float
    size_t getSize(size_t nt, size_t ns);
    void readHO(unsigned char * data);
    void readDO(size_t start, size_t sz, unsigned char * dos, size_t ns);
    void readDODF(size_t start, size_t sz, float * data, size_t ns);
    void readDOMD(size_t start, size_t sz, unsigned char * data, size_t ns);
    void writeHO(unsigned char * data);
    void writeDO(size_t start, size_t sz, unsigned char * dos, size_t ns);
    void writeDODF(size_t start, size_t sz, float * data, size_t ns);
    void writeDOMD(size_t start, size_t sz, unsigned char * data, size_t ns);
};

namespace SEGSz
{
enum class Size : size_t
{
    HO = 3600,
    Md = 240
};

constexpr size_t getMDSz()
{
    return static_cast<size_t>(Size::Md);
}
constexpr size_t getHOSz()
{
    return static_cast<size_t>(Size::HO);
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
}
}}
#endif
