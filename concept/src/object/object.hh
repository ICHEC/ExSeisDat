#ifndef OBJECT_INCLUDE_GUARD
#define OBJECT_INCLUDE_GUARD
#include <memory>
#include "block/block.hh"
namespace PIOL { namespace Obj {
class ObjectLayer
{
    protected :
    typedef PIOL::Block::BlockLayer Bl;
    typedef PIOL::Block::Type Bt;
    typedef PIOL::Block::MPI::Blck<MPI_Status> mBl;
    std::unique_ptr<Bl> pBlock;
    public :
    static size_t getSize(size_t nt, size_t ns) __attribute__((pure));

    virtual size_t getFileSz(void)
    {
        return pBlock->getFileSz();
    }
    virtual void readHO(char * data) = 0;
    virtual void readDO(size_t start, size_t sz, char * dos, size_t ns) = 0;
    virtual void readDODF(size_t start, size_t sz, char ** data, size_t ns) = 0;
    virtual void readDOMD(size_t start, size_t sz, char ** data, size_t ns) = 0;

    virtual void writeHO(char * data) = 0;
    virtual void writeDO(size_t start, size_t sz, char * dos, size_t ns) = 0;
    virtual void writeDODF(size_t start, size_t sz, char ** data, size_t ns) = 0;
    virtual void writeDOMD(size_t start, size_t sz, char ** data, size_t ns) = 0;
};
}}

#include "objsegy.hh"
#endif

