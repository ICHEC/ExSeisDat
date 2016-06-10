#ifndef MPIIO_GUARD
#define MPIIO_GUARD

#include <mpi.h>
#include <limits>
#include <vector>
#include <typeinfo>

namespace PIOL { namespace Block { namespace MPI {

extern MPI_File open(MPI_Comm, std::string, int);
extern size_t getFileSz(MPI_File);
extern void growFile(MPI_File, size_t);
extern void printErr(int, MPI_Status *, std::string);

template <typename U>
using Fp = int (*)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, U *);

//typedef int (* MPI_Fp)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, MPI_Status *);
//typedef int (* MPI_IFp)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, MPI_Request *);

template <typename T>
constexpr MPI_Offset getLim()
{
    //If you aren't (4096 - Chunk)/Chunk from the limit, intel mpi breaks.
    //Probably something to do with pages.
    //return MPI_Offset((std::numeric_limits<int>::max() - (4096U - sizeof(T))) / sizeof(T));
    return MPI_Offset((std::numeric_limits<int>::max() - (4096U - sizeof(T))) / sizeof(T));
}

template <typename T>
constexpr MPI_Datatype Type()
{
    return (typeid(T) == typeid(double)             ? MPI_DOUBLE
         : (typeid(T) == typeid(long double)        ? MPI_LONG_DOUBLE
         : (typeid(T) == typeid(char)               ? MPI_CHAR
         : (typeid(T) == typeid(unsigned char)      ? MPI_UNSIGNED_CHAR
         : (typeid(T) == typeid(int)                ? MPI_INT
         : (typeid(T) == typeid(long int)           ? MPI_LONG
         : (typeid(T) == typeid(unsigned long int)  ? MPI_UNSIGNED_LONG
         : (typeid(T) == typeid(unsigned int)       ? MPI_UNSIGNED
         : (typeid(T) == typeid(long long int)      ? MPI_LONG_LONG_INT
         : (typeid(T) == typeid(float)              ? MPI_FLOAT
         : (typeid(T) == typeid(signed short)       ? MPI_SHORT
         : (typeid(T) == typeid(unsigned short)     ?  MPI_UNSIGNED_SHORT
         : MPI_BYTE))))))))))));
}
template <class T, class U=T>
void setView(MPI_File file, MPI_Offset offset = 0)
{
    MPI_Info info = MPI_INFO_NULL;
    int err = MPI_File_set_view(file, offset, Type<T>(), Type<U>(), "native", info);
    printErr(err, NULL, "MPI_File_set_view failure\n"); 
}

template <typename T, typename U = MPI_Status> 
U MPIIO(Fp<U> fn, MPI_File & file, MPI_Offset offset, T * d, size_t sz)
{
    U arg;
    int err = MPI_SUCCESS;
    auto max = getLim<T>();
    MPI_Offset q = sz / max;
    MPI_Offset r = sz % max;

    std::cout << "off = " << offset << " q = " << q << " r = " << r << "r+q*max = " << r+q*max << std::endl 
             << "max = " << max << " sz " << sz << "tsize " << sizeof(T) << std::endl;

    for (MPI_Offset i = 0; i < q; i++)
    {
        std::cout << " fn(file, " << offset + i*max << ", &d[i*max], " << max << ", Type<T>(), &arg);\n";
        err = fn(file, offset + i*max, &d[i*max], max, Type<T>(), &arg);
        if (err != MPI_SUCCESS)
            break;
    }

    if (err == MPI_SUCCESS)
    {
        std::cout << " fn(file, " << offset + q*max << ", &d[q*max], " << r << ", Type<T>(), &arg);\n";
        err = fn(file, offset + q*max, &d[q*max], r, Type<T>(), &arg);
    }
    else
        std::cerr << "Error with MPIIO\n";

    printErr(err, (typeid(T) == typeid(MPI_Status) ? &arg : NULL), " MPIIO Failure\n");

    return arg;
}

template <typename T, typename U = MPI_Status> 
U MPIIO(Fp<U> fn, MPI_File & file, MPI_Offset offset, std::vector<T> & d)
{
    return MPIIO<T, U>(fn, file, offset, &d.front(), d.size());
}
}}}
#endif
