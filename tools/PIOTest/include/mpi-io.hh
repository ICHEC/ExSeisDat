#ifndef MPIIO_GUARD
#define MPIIO_GUARD
#include <mpi.h>
#include <limits>
#include <vector>
#include <typeinfo>
typedef double real;
namespace io
{
extern MPI_File Open(MPI_Comm Comm, std::string Name, int Mode);
typedef int (* MPI_Fp)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, MPI_Status *);
//extern void MPIIO(MPI_Fp Fn, MPI_File & File, MPI_Offset Offset, MPI_Offset Sz, real * D);
extern void MPIErr(int Err, MPI_Status * Stat, std::string Msg);

constexpr MPI_Offset MPILimits(const size_t Chunk)
{
    //If you aren't (4096 - Chunk)/Chunk from the limit, intel mpi breaks.
    //Probably something to do with pages.
    return MPI_Offset((std::numeric_limits<int>::max() - (4096U - Chunk)) / Chunk);
}

template <typename T>
constexpr MPI_Datatype MPIType()
{
    return (typeid(T) == typeid(double) ? MPI_DOUBLE
        : (typeid(T) == typeid(long double) ? MPI_LONG_DOUBLE
        : (typeid(T) == typeid(char) ? MPI_CHAR
        : (typeid(T) == typeid(unsigned char) ? MPI_UNSIGNED_CHAR
        : (typeid(T) == typeid(int) ? MPI_INT
        : (typeid(T) == typeid(long int) ? MPI_LONG
        : (typeid(T) == typeid(unsigned long int) ? MPI_UNSIGNED_LONG
        : (typeid(T) == typeid(long long int) ? MPI_LONG_LONG_INT
        : (typeid(T) == typeid(float) ? MPI_FLOAT
        : (typeid(T) == typeid(signed short) ? MPI_SHORT
        : (typeid(T) == typeid(unsigned short) ?  MPI_UNSIGNED_SHORT
        : MPI_BYTE)))))))))));
}

template <class T, class U=T>
void View(MPI_File & File, MPI_Offset Offset = 0)
{
    MPI_Info Info = MPI_INFO_NULL;
    int Err = MPI_File_set_view(File, Offset, MPIType<T>(), MPIType<U>(), "native", Info);
    MPIErr(Err, NULL, "MPI_File_set_view failure\n"); 

}

typedef int (* MPI_Fp)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, MPI_Status *);
template <typename T> 
void MPIIO(MPI_Fp Fn, MPI_File & File, MPI_Offset Offset, std::vector<T> & D)
{
    MPI_Offset Sz = D.size(); //Smaller, but huge
    MPI_Status Stat;
    int Err;
    auto MPIMax = MPILimits(sizeof(T));
    MPI_Datatype typ = MPIType<T>();
    MPI_Offset q = Sz / MPIMax;
    MPI_Offset r = Sz % MPIMax;

    for (MPI_Offset i = 0; i < q; i++)
    {
        Err = Fn(File, Offset + i*MPIMax, &D[i*MPIMax], MPIMax, typ, &Stat);
        if (Err == MPI_ERR_IN_STATUS)
            break;
    }

    if (Err != MPI_ERR_IN_STATUS)
        Err = Fn(File, Offset + Sz-r, &D[Sz-r], r, typ, &Stat);

    MPIErr(Err, &Stat, "MPIIO Failure\n"); 
}

extern template void MPIIO<real>(MPI_Fp Fn, MPI_File & File, MPI_Offset Offset, std::vector<real> & D);
}
#endif
