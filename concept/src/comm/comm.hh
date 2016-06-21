#ifndef PIOLCOMM_INCLUDE_GUARD
#define PIOLCOMM_INCLUDE_GUARD
namespace PIOL {
namespace Comms
{
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

}
}
#endif

