#ifndef PIOLOBJ_INCLUDE_GUARD
#define PIOLOBJ_INCLUDE_GUARD
#include <memory>
#include "global.hh"
#include "block/block.hh"
namespace PIOL { namespace Obj {
class Interface
{
    protected :
    typedef PIOL::Block::Interface Bl;
    typedef PIOL::Block::Type Bt;
    std::unique_ptr<Bl> block;

    public :
    virtual size_t getSize(size_t, size_t) = 0;

    virtual size_t getFileSz(void)
    {
        return block->getFileSz();
    }
    virtual void setFileSz(size_t nt, size_t ns)
    {
        block->setFileSz(getSize(nt, ns));
    }

    virtual void readHO(uchar *) = 0;
    virtual void readDO(size_t, size_t, uchar *, size_t) = 0;
    virtual void readDODF(size_t, size_t, float *, size_t) = 0;
    virtual void readDOMD(size_t, size_t, uchar *, size_t) = 0;

    virtual void writeHO(uchar *) = 0;
    virtual void writeDO(size_t, size_t, uchar *, size_t) = 0;
    virtual void writeDODF(size_t, size_t, float *, size_t) = 0;
    virtual void writeDOMD(size_t, size_t, uchar *, size_t) = 0;
};
}}
#endif

