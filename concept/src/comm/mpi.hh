#ifndef PIOLCOMMMPI_INCLUDE_GUARD
#define PIOLCOMMMPI_INCLUDE_GUARD
#include <vector>
#include <functional>
#include <mpi.h>
#include "comm/comm.hh"
namespace PIOL { namespace Comms {

//TODO: Consolidate with Block
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
         : (typeid(T) == typeid(unsigned long int)  ? MPI_UNSIGNED_LONG
         : (typeid(T) == typeid(unsigned int)       ? MPI_UNSIGNED
         : (typeid(T) == typeid(long long int)      ? MPI_LONG_LONG_INT
         : (typeid(T) == typeid(float)              ? MPI_FLOAT
         : (typeid(T) == typeid(signed short)       ? MPI_SHORT
         : (typeid(T) == typeid(unsigned short)     ? MPI_UNSIGNED_SHORT
         : MPI_BYTE))))))))))));
}
class MPI : public Comms::Interface
{
    private :
    MPI_Comm comm;
    bool init;
    public :
    MPI(MPI_Comm Comm, bool Init = true) : init(Init)
    {
        comm = Comm;
        if (init)
        {
            //Quote from MPI 3.1 specification: "The version for ISO C accepts the argc and argv that are provided by the arguments to main or NULL"
            int err = MPI_Init(NULL, NULL);
            //PIOL::Block::MPI::printErr(err, NULL, "MPI_Init failure\n"); 
        }
        int irank;
        int inumRank;
        MPI_Comm_rank(comm, &irank);
        MPI_Comm_size(comm, &inumRank);
        rank = irank;
        numRank = inumRank;
    }
    ~MPI(void)
    {
        if (init)
        {
            int err = MPI_Finalize();
            //PIOL::Block::MPI::printErr(err, NULL, "MPI_Finalize failure\n");
        }
    }
    MPI_Comm getComm()
    {
        return comm;
    }

    template <typename T>
    T reduce(T val, std::function<const T & (const T &, const T &)> reducOp)
    {
        std::vector<T> vals(getNumRank());
        vals[getRank()] = val;

        MPI_Allgather(MPI_IN_PLACE, 0, Type<T>(), &vals, getNumRank(), Type<T>(), getComm());

        val = vals[0];
        for (size_t i = 0; i < getNumRank(); i++)
            val = reducOp(val, vals[i]);
        return val;
    }
};


}}
#endif

