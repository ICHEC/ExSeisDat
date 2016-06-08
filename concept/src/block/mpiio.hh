#ifndef MPIIO_GUARD
#define MPIIO_GUARD

#include <mpi.h>
#include <limits>
#include <vector>
#include <typeinfo>

namespace PIOL { namespace Block { namespace MPI {

extern MPI_File open(MPI_Comm, std::string, int);
extern size_t getFileSz(MPI_File);
extern void setFileSz(MPI_File, size_t);
extern void printErr(int, MPI_Status *, std::string);

template <typename U>
using Fp = int (*)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, U *);

//typedef int (* MPI_Fp)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, MPI_Status *);
//typedef int (* MPI_IFp)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, MPI_Request *);

constexpr MPI_Offset getLimits(const size_t chunk)
{
    //If you aren't (4096 - Chunk)/Chunk from the limit, intel mpi breaks.
    //Probably something to do with pages.
    return MPI_Offset((std::numeric_limits<int>::max() - (4096U - chunk)) / chunk);
}

template <typename T>
constexpr MPI_Datatype Type()
{
    return (typeid(T) == typeid(double) ? MPI_DOUBLE
        : (typeid(T) == typeid(long double) ? MPI_LONG_DOUBLE
        : (typeid(T) == typeid(char) ? MPI_CHAR
        : (typeid(T) == typeid(unsigned char) ? MPI_UNSIGNED_CHAR
        : (typeid(T) == typeid(int) ? MPI_INT
        : (typeid(T) == typeid(long int) ? MPI_LONG
        : (typeid(T) == typeid(unsigned long int) ? MPI_UNSIGNED_LONG
        : (typeid(T) == typeid(unsigned int) ? MPI_UNSIGNED
        : (typeid(T) == typeid(long long int) ? MPI_LONG_LONG_INT
        : (typeid(T) == typeid(float) ? MPI_FLOAT
        : (typeid(T) == typeid(signed short) ? MPI_SHORT
        : (typeid(T) == typeid(unsigned short) ?  MPI_UNSIGNED_SHORT
        : MPI_BYTE))))))))))));
}
template <class T, class U=T>
void setView(MPI_File & file, MPI_Offset offset = 0)
{
    MPI_Info info = MPI_INFO_NULL;
    int err = MPI_File_set_view(file, offset, Type<T>(), Type<U>(), "native", info);
    printErr(err, NULL, "MPI_File_set_view failure\n"); 
}

template <typename T = float, typename U = MPI_Status> 
U MPIIO(Fp<U> fn, MPI_File & file, MPI_Offset offset, T * d, size_t sz)
{
    U arg;
    int err;
    auto MPIMax = getLimits(sizeof(T));
    MPI_Offset q = sz / MPIMax;
    MPI_Offset r = sz % MPIMax;

    for (MPI_Offset i = 0; i < q; i++)
    {
        err = fn(file, offset + i*MPIMax, &d[i*MPIMax], MPIMax, Type<T>(), &arg);
        if (err != MPI_SUCCESS)
            break;
    }

    if (err == MPI_SUCCESS)
        err = fn(file, offset + sz-r, &d[sz-r], r, Type<T>(), &arg);
    
    printErr(err, (typeid(T) == typeid(MPI_Status) ? &arg : NULL), "MPIIO Failure\n");

    return arg;
}

template <typename T = float, typename U = MPI_Status> 
U MPIIO(Fp<U> fn, MPI_File & file, MPI_Offset offset, std::vector<T> & d)
{
    return MPIIO<T, U>(fn, file, offset, &d.front(), d.size());
}

}}}
#endif
