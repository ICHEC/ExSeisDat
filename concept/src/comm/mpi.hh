#ifndef PIOLCOMMMPI_INCLUDE_GUARD
#define PIOLCOMMMPI_INCLUDE_GUARD
#include "comm/comm.hh"
#include <vector>
#include <functional>
#include <mpi.h>
#include <cassert>
#include "global.hh"

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
         : (typeid(T) == typeid(size_t)             ? MPI_UNSIGNED_LONG ///TODO: Watch out for this one!
         : (typeid(T) == typeid(unsigned long int)  ? MPI_UNSIGNED_LONG
         : (typeid(T) == typeid(unsigned int)       ? MPI_UNSIGNED
         : (typeid(T) == typeid(long long int)      ? MPI_LONG_LONG_INT
         : (typeid(T) == typeid(float)              ? MPI_FLOAT
         : (typeid(T) == typeid(signed short)       ? MPI_SHORT
         : (typeid(T) == typeid(unsigned short)     ? MPI_UNSIGNED_SHORT
         : MPI_BYTE)))))))))))));
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
        assert(getRank() < getNumRank());
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
    std::vector<T> gatherScalar(T val)
    {
        std::vector<T> vals(getNumRank());
        vals[getRank()] = val;

        MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, vals.data(), 1, Type<T>(), getComm());
        return vals;
    }

    template <typename T>
    T reduce(T val, std::function<const T & (const T &, const T &)> reducOp)
    {
        std::vector<T> vals = gatherScalar(val);

        val = vals[0];
        for (size_t i = 0; i < getNumRank(); i++)
            val = reducOp(val, vals[i]);
        return val;
    }

    template <typename T>
    size_t reduce(T val, size_t offset, size_t num, std::function<const T & (const T &, const T &)> reducOp)
    {
        std::vector<T> vals = gatherScalar(val);
        std::vector<size_t> nums = gatherScalar(num);
        std::vector<size_t> offsets = gatherScalar(offset);

        int test;
        MPI_Type_size(MPI_UNSIGNED_LONG, &test);
        assert(test == sizeof(size_t));
        assert(vals.size() > 0);
        assert(nums.size() > 0);
        assert(offsets.size() > 0);
        size_t rank = 0;
        val = vals[0];

        for (size_t i = 1; i < vals.size(); i++)
        {
            T newVal = reducOp(val, vals[i]);
            if (newVal != val)
            {
                val = newVal;
                rank = i;
            }
        }

        return nums[rank] + offsets[rank];
    }

};


}}
#endif

