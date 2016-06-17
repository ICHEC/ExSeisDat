#ifndef PIOLBLOCKLAYER_INCLUDE_GUARD
#define PIOLBLOCKLAYER_INCLUDE_GUARD

#include <mpi.h>
#include "comm/comm.hh"
namespace PIOL { namespace Block {

class Interface
{
    protected :
    Comms::Interface & comm;
    public :
    Interface(Comms::Interface & Comm) : comm(Comm)
    {

    }
    virtual void setFileSz(size_t) = 0;
    virtual size_t getFileSz() = 0;
    virtual void setView(size_t) = 0;

    virtual void readData(size_t, float *, size_t) = 0;
    virtual void readData(size_t, unsigned char *, size_t) = 0;
    virtual void writeData(size_t, float *, size_t) = 0;
    virtual void writeData(size_t, unsigned char *, size_t) = 0;
};
enum class Type
{
    MPI
} block;
}}

#include "blckmpiio.hh"

#endif
