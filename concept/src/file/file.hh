#ifndef PIOLFILEFILE_INCLUDE_GUARD
#define PIOLFILEFILE_INCLUDE_GUARD
#include <vector>
#include <cassert>
#include <string>
#include <iostream>
#include <mpi.h>
#include "object/object.hh"
typedef float real;

namespace PIOL { namespace File {
class FileLayer
{
    protected:
    std::string note;
    real inc;
    size_t ns;
    size_t nt;
    public :

    size_t getNs()
    {
        return ns;
    }
    size_t getNt()
    {
        return nt;
    }
    real getInc()
    {
        return inc;
    }
    virtual void getCoord() = 0;
    virtual void setCoord() = 0;
    virtual void getTraces() = 0;
    virtual void setTraces() = 0;
};
#include "SEGY.hh"
}}
#endif
