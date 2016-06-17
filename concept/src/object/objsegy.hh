#ifndef OBJSEGY_INCLUDE_GUARD
#define OBJSEGY_INCLUDE_GUARD

#include "object/object.hh"
#include "object/SEGY.hh"
#include "block/blckmpiio.hh"

namespace PIOL { namespace Obj { namespace SEGY {
class Interface : public PIOL::Obj::Interface
{
    typedef PIOL::Block::MPI::Interface<MPI_Status> mBl;
    public : 
    Interface(Comms::Interface & comm, std::string name, Bt bType)
    {
        switch (bType)
        {
            case Bt::MPI :
            default :
            {
                block = std::make_unique<mBl>(comm, name);
            }
            break;
        }
    }

    //Assumes float
    size_t getSize(size_t nt, size_t ns)
    {
        return getHOSz() + nt*getDOSz<float>(ns);
    }

    void readHO(unsigned char * data)
    {
        block->readData(0, data, getHOSz());
    }

    void readDO(size_t start, size_t sz, unsigned char * dos, size_t ns)
    {
        block->readData(start, dos, sz*getDOSz<float>(ns));
    }

    void readDODF(size_t start, size_t sz, float * data, size_t ns)
    {
        for (size_t i = 0; i < sz; i++)
            block->readData(getDODFLoc<float>(start+i, ns), &data[i*ns], ns);
    }

    void readDOMD(size_t start, size_t sz, unsigned char * data, size_t ns)
    {
        for (size_t i = 0; i < sz; i++)
            block->readData(getDOLoc<float>(start+i, ns), &data[i*getMDSz()], getMDSz());
    }

    void writeHO(unsigned char * data)
    {
        block->writeData(0, data, getMDSz());
    }

    void writeDO(size_t start, size_t sz, unsigned char * dos, size_t ns)
    {
        block->writeData(start, dos, sz*getDOSz<float>(ns));
    }

    void writeDODF(size_t start, size_t sz, float * data, size_t ns)
    {
        for (size_t i = 0; i < sz; i++)
            block->writeData(getDODFLoc<float>(start+i, ns), &data[i*ns], ns);
    }

    void writeDOMD(size_t start, size_t sz, unsigned char * data, size_t ns)
    {
        for (size_t i = 0; i < sz; i++)
            block->writeData(getDOLoc<float>(start+i, ns), &data[i*getMDSz()], getMDSz());
    }


};
}}}
#endif
