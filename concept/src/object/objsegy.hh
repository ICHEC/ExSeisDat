#ifndef OBJSEGY_INCLUDE_GUARD
#define OBJSEGY_INCLUDE_GUARD

#include "SEGY.hh"
#include "block/block.hh"

namespace PIOL { namespace Obj { namespace SEGY {
class Interface : public PIOL::Obj::Interface
{
    public : 
    Interface(MPI_Comm comm, std::string name, Bt bType)
    {
        switch (bType)
        {
            case Bt::MPI :
            default :
            {
                this->pBlock = std::make_unique<mBl>(comm, name);
            }
            break;
        }

    }

    //Assumes float
    static size_t getSize(size_t nt, size_t ns)
    {
        return getHOSz() + nt*getDOSz<float>(ns);
    }
    
    void readHO(unsigned char * data)
    {
        this->pBlock->readData(0, data, getHOSz());
    }

    void readDO(size_t start, size_t sz, unsigned char * dos, size_t ns)
    {
        this->pBlock->readData(start, dos, sz*getDOSz<float>(ns));
    }

    void readDODF(size_t start, size_t sz, float * data, size_t ns)
    {
        for (size_t i = 0; i < sz; i++)
            this->pBlock->readData(getDODFLoc<float>(start+i, ns), &data[i*ns], ns);
    }

    void readDOMD(size_t start, size_t sz, unsigned char * data, size_t ns)
    {
        for (size_t i = 0; i < sz; i++)
            this->pBlock->readData(getDOLoc<float>(start+i, ns), &data[i*getMDSz()], getMDSz());
    }

    void writeHO(unsigned char * data)
    {
        this->pBlock->writeData(0, data, getMDSz());
    }

    void writeDO(size_t start, size_t sz, unsigned char * dos, size_t ns)
    {
        this->pBlock->writeData(start, dos, sz*getDOSz<float>(ns));
    }

    void writeDODF(size_t start, size_t sz, float * data, size_t ns)
    {
        for (size_t i = 0; i < sz; i++)
            this->pBlock->writeData(getDODFLoc<float>(start+i, ns), &data[i*ns], ns);
    }

    void writeDOMD(size_t start, size_t sz, unsigned char * data, size_t ns)
    {
        for (size_t i = 0; i < sz; i++)
            this->pBlock->writeData(getDOLoc<float>(start+i, ns), &data[i*getMDSz()], getMDSz());
    }
};
}}}
#endif
