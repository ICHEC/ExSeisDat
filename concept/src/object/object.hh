#include "objsegy.hh"

namespace PIOL { namespace Obj { namespace SEGY {
class ObjectLayer
{
    typedef PIOL::Block::BlockLayer Bl;
    typedef PIOL::Block::Type Bt;
    typedef PIOL::Block::MPI::Blck<MPI_Status> mBl;

    virtual void readHO(Bl & block, char * data) = 0;
    virtual void readDO(Bl & block, size_t start, size_t sz, char * dos, size_t ns) = 0;
    virtual void readDODF(Bl & block, size_t start, size_t sz, char ** data, size_t ns) = 0;
    virtual void readDOMD(Bl & block, size_t start, size_t sz, char ** data, size_t ns) = 0;
    virtual void writeHO(Bl & block, char * data) = 0;
    virtual void writeDO(Bl & block, size_t start, size_t sz, char * dos, size_t ns) = 0;
    virtual void writeDODF(Bl & block, size_t start, size_t sz, char ** data, size_t ns) = 0;
    virtual void writeDOMD(Bl & block, size_t start, size_t sz, char ** data, size_t ns) = 0;
};
}}}

