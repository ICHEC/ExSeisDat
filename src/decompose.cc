////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date May 2017
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/decompose.h"


extern "C" {
PIOL_Range PIOL_decompose(size_t sz, size_t numRank, size_t rank)
{
    size_t q     = sz / numRank;
    size_t r     = sz % numRank;
    size_t start = q * rank + std::min(rank, r);
    return {start, std::min(sz - start, q + (rank < r))};
}
}

namespace PIOL {

/*! An overload for a common decomposition case. Perform a decomposition of
 *  traces so that the load is optimally balanced.
 *  @param[in] piol The piol object
 *  @param[in] file A read file object. This is used to find the number of
 *             traces.
 *  @return Return a pair (offset, size).
 *          The first element is the offset for the local process,
 *          the second is the size for the local process.
 *  @todo DELETE THIS DOC
 */
Range decompose(ExSeisPIOL* piol, ReadInterface* file)
{
    return decompose(
      file->readNt(), piol->comm->getNumRank(), piol->comm->getRank());
}

}  // namespace PIOL
