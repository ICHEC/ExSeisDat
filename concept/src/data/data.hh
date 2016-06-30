#ifndef PIOLBLOCK_INCLUDE_GUARD
#define PIOLBLOCK_INCLUDE_GUARD
#include <memory>
#include <mpi.h>
#include "comm/comm.hh"
namespace PIOL { namespace Data {

class Interface
{
    protected :
    std::shared_ptr<Comms::Interface> comm;
    public :
    Interface(std::shared_ptr<Comms::Interface> comm_) : comm(comm_)
    {
    }
    virtual void setFileSz(size_t) = 0;
    virtual size_t getFileSz(void) = 0;
    virtual void setView(size_t) = 0;

    virtual void readData(size_t, float *, size_t) = 0;
    virtual void readData(size_t, uchar *, size_t) = 0;
    virtual void writeData(size_t, float *, size_t) = 0;
    virtual void writeData(size_t, uchar *, size_t) = 0;
};

class Options
{


};

enum class Type
{
    MPIIO
};
typedef PIOL::Data::Type Bt;
}}

#endif
