#include <memory>
#include "comm/mpi.hh"
#include "object/objsegy.hh"
#include "data/datampiio.hh"

using namespace PIOL::Obj::SEGSz;
namespace PIOL { namespace Obj {

SEGY::SEGY(std::shared_ptr<Comms::Interface> comm, std::string name, Bt bType)
{
    typedef PIOL::Data::MPIIO mBl;
    switch (bType)
    {
        case Bt::MPIIO :
        default :
        {
            //Make sure we are using MPI as our communicator for MPI-IO to make sense
            auto mpicomm = std::dynamic_pointer_cast<Comms::MPI>(comm);
            block = std::make_unique<mBl>(mpicomm, name);
        }
        break;
    }
}

//Assumes float
size_t SEGY::getSize(size_t nt, size_t ns)
{
    return getHOSz() + nt*getDOSz<float>(ns);
}

void SEGY::readHO(unsigned char * data)
{
    block->readData(0, data, getHOSz());
}

void SEGY::readDO(size_t start, size_t sz, unsigned char * dos, size_t ns)
{
    block->readData(getDOLoc<float>(start, ns), dos, sz*getDOSz<float>(ns));
}

void SEGY::readDODF(size_t start, size_t sz, float * data, size_t ns)
{
    for (size_t i = 0; i < sz; i++)
        block->readData(getDODFLoc<float>(start+i, ns), &data[i*ns], ns);
}

void SEGY::readDOMD(size_t start, size_t sz, unsigned char * data, size_t ns)
{
    for (size_t i = 0; i < sz; i++)
        block->readData(getDOLoc<float>(start+i, ns), &data[i*getMDSz()], getMDSz());
}

void SEGY::writeHO(unsigned char * data)
{
    block->writeData(0, data, getHOSz());
}

void SEGY::writeDO(size_t start, size_t sz, unsigned char * dos, size_t ns)
{
    block->writeData(getDOLoc<float>(start, ns), dos, sz*getDOSz<float>(ns));
}

void SEGY::writeDODF(size_t start, size_t sz, float * data, size_t ns)
{
    for (size_t i = 0; i < sz; i++)
        block->writeData(getDODFLoc<float>(start+i, ns), &data[i*ns], ns);
}

void SEGY::writeDOMD(size_t start, size_t sz, unsigned char * data, size_t ns)
{
    for (size_t i = 0; i < sz; i++)
        block->writeData(getDOLoc<float>(start+i, ns), &data[i*getMDSz()], getMDSz());
}
}}
