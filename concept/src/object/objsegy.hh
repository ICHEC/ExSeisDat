#include <memory>
#include "SEGY.hh"
#include "block/block.hh"

namespace PIOL { namespace Obj { namespace SEGY {
class ObjSEGY : public ObjectLayer
{
    typedef PIOL::Block::BlockLayer Bl;
    typedef PIOL::Block::Type Bt;
    typedef PIOL::Block::MPI::Blck<MPI_Status> mBl;
    
    std::unique_ptr<Bl> pBlock;
    public : 
    ObjSEGY(MPI_Comm comm, std::string name, Bt bType)
    {
        switch (bType)
        {
            case Bt::MPI :
            default :
            {
                pBlock = std::make_unique<mBl>(comm, name);
            }
            break;
        }

    }

    void readHO(Bl & block, char * data)
    {
        block.readData(0, data, getMDSz());
    }

    void readDO(Bl & block, size_t start, size_t sz, char * dos, size_t ns)
    {
        block.readData(start, dos, sz*getDOSz<char>(ns));
    }

    void readDODF(Bl & block, size_t start, size_t sz, char ** data, size_t ns)
    {
        for (size_t i = 0; i < sz; i++)
            block.readData(getDODFLoc<char>(start+i, ns), data[i], ns);
    }

    void readDOMD(Bl & block, size_t start, size_t sz, char ** data, size_t ns)
    {
        for (size_t i = 0; i < sz; i++)
            block.readData(getDOLoc<char>(start+i, ns), data[i], getMDSz());
    }

    void writeHO(Bl & block, char * data)
    {
        block.writeData(0, data, getMDSz());
    }

    void writeDO(Bl & block, size_t start, size_t sz, char * dos, size_t ns)
    {
        block.writeData(start, dos, sz*getDOSz<float>(ns));
    }

    void writeDODF(Bl & block, size_t start, size_t sz, char ** data, size_t ns)
    {
        for (size_t i = 0; i < sz; i++)
            block.writeData(getDODFLoc<char>(start+i, ns), data[i], ns);
    }

    void writeDOMD(Bl & block, size_t start, size_t sz, char ** data, size_t ns)
    {
        for (size_t i = 0; i < sz; i++)
            block.writeData(getDOLoc<char>(start+i, ns), data[i], getMDSz());
    }
};
}}}

