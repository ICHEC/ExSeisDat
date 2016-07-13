#ifndef PIOLSHAREDMPI_INCLUDE_GUARD
#define PIOLSHAREDMPI_INCLUDE_GUARD
#include <mpi.h>
#include "anc/piol.hh"
#include "anc/log.hh"
namespace PIOL {
extern void printErr(ExSeisPIOL & piol, const std::string file, const Log::Layer layer, const int err, const MPI_Status * stat, std::string msg);

template <typename T>
#ifndef __ICC
constexpr
#endif
MPI_Datatype Type()
{
    return (typeid(T) == typeid(double)             ? MPI_DOUBLE
         : (typeid(T) == typeid(long double)        ? MPI_LONG_DOUBLE
         : (typeid(T) == typeid(char)               ? MPI_CHAR
         : (typeid(T) == typeid(uchar)              ? MPI_UNSIGNED_CHAR
         : (typeid(T) == typeid(int)                ? MPI_INT
         : (typeid(T) == typeid(long int)           ? MPI_LONG
         : (typeid(T) == typeid(size_t)             ? MPI_UNSIGNED_LONG ///TODO: Watch out for this one!
         : (typeid(T) == typeid(unsigned long int)  ? MPI_UNSIGNED_LONG
         : (typeid(T) == typeid(unsigned int)       ? MPI_UNSIGNED
         : (typeid(T) == typeid(long long int)      ? MPI_LONG_LONG_INT
         : (typeid(T) == typeid(float)              ? MPI_FLOAT
         : (typeid(T) == typeid(signed short)       ? MPI_SHORT
         : (typeid(T) == typeid(unsigned short)     ? MPI_UNSIGNED_SHORT
         : MPI_BYTE)))))))))))));
}

template <typename T>
constexpr MPI_Offset getLim()
{
    //If you aren't (4096 - Chunk)/Chunk from the limit, intel mpi breaks.
    //Probably something to do with pages.
    //return MPI_Offset((std::numeric_limits<int>::max() - (4096U - sizeof(T))) / sizeof(T));
    return MPI_Offset((std::numeric_limits<int>::max() - (4096U - sizeof(T))) / sizeof(T));
}
}
#endif
