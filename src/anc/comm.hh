#ifndef PIOLCOMM_INCLUDE_GUARD
#define PIOLCOMM_INCLUDE_GUARD
#include <cstddef>
namespace PIOL {namespace Comm {
class Interface
{
    protected :
    size_t rank;
    size_t numRank;
    public :
    virtual size_t getRank()
    {
        return rank;
    }
    size_t getNumRank()
    {
        return numRank;
    }
};

enum class Type : size_t
{
    MPI
};

struct Opt
{
    Type type = Type::MPI;
    virtual Type getType(void)
    {
        return type;
    }
};
}}


#endif

