#ifndef PIOLBLOCKLAYER_INCLUDE_GUARD
#define PIOLBLOCKLAYER_INCLUDE_GUARD

#include <mpi.h>
namespace PIOL { namespace Block {

class BlockLayer
{
    protected :
    MPI_Comm comm;
    public :
    BlockLayer(MPI_Comm Comm) : comm(Comm)
    {

    }
    virtual void growFile(size_t) = 0;
    virtual size_t getFileSz() = 0;
    virtual void setView(size_t) = 0;

    virtual void readData(size_t, float *, size_t) = 0;
    virtual void readData(size_t, char *, size_t) = 0;
    virtual void writeData(size_t, float *, size_t) = 0;
    virtual void writeData(size_t, char *, size_t) = 0;
};
enum class Type
{
    MPI
} block;
}}

#include "blckmpiio.hh"

#endif
