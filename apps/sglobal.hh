////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_PIOL_FOUR_D_SGLOBAL_HH
#define EXSEIS_PIOL_FOUR_D_SGLOBAL_HH

#include "exseis/utils/communicator/Communicator.hh"
#include "exseis/utils/decomposition/block_decomposition.hh"
#include "exseis/utils/types/typedefs.hh"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <vector>

#include <stddef.h>

namespace exseis {
namespace apps {

/*! @brief Decomposition which takes account that the data may come in logical
 *         blocks
 *
 *  @param[in] sz       The size of the problem.
 *  @param[in] bsz      The block size
 *  @param[in] num_rank The total number of ranks.
 *  @param[in] rank     The rank during the function call, generally the rank of
 *                      the calling MPI process.
 *  @param[in] off      The offset (optional)
 *
 *  @return Return an extent, a starting point (generally for a 'for' loop) and
 *          the number of iterations.
 */
inline exseis::Contiguous_decomposition block_decomp(
    size_t sz, size_t bsz, size_t num_rank, size_t rank, size_t off)
{
    // Size of the first block
    size_t rstart = bsz - off % bsz;
    // Size of the last block
    size_t rend = (off + sz) % bsz;
    // The total number of blocks, +2 accounts for start+end
    size_t bcnt = (sz - rstart - rend) / bsz + 2;

    // Make sure there isn't any leftover, this shouldn't be possible
    assert(!((sz - rstart - rend) % bsz));

    // Do a regular decomposition of the blocks
    auto newdec = exseis::block_decomposition(bcnt, num_rank, rank);

    newdec.global_offset *= bsz;
    newdec.local_size *= bsz;
    if (newdec.local_size == 0) {
        return {sz, 0};
    }

    // Now we compensate for the fact that the start and end block sizes can be
    // different.
    if (rank == 0) {
        // If the rank is zero, we shrink the first block by the leftover
        newdec.local_size -= bsz - rstart;
    }
    else {
        // The subtraction above means every block is shifted
        newdec.global_offset -= bsz - rstart;
    }

    // The last rank with work must remove the leftover of its last block
    if (newdec.local_size != 0
        && ((newdec.global_offset + newdec.local_size) > sz)) {
        newdec.local_size -= bsz - rend;
    }

    return newdec;
}

/*! @brief Unequal decomposition, weights towards the lower ranks. Useful for
 *         testing purposes
 *
 *  @param[in] communicator  the communicator to decompose over
 *  @param[in] sz       The size of the problem.
 *  @param[in] num_rank The total number of ranks.
 *  @param[in] rank     The rank during the function call, generally the rank of
 *                      the calling MPI process.
 *
 *  @return Return an extent, a starting point (generally for a 'for' loop) and
 *          the number of iterations.
 */
inline std::vector<size_t> lobdecompose(
    const Communicator& communicator, size_t sz, size_t num_rank, size_t rank)
{
    double total = (num_rank * (num_rank + 1U)) / 2U;
    rank++;
    size_t lnt = std::lround(double(sz * rank) / total);

    auto rem = sz - communicator.sum(lnt);
    if (rank == 1) {
        lnt += rem;
    }

    auto nts       = communicator.gather<size_t>(lnt);
    size_t biggest = *std::max_element(nts.begin(), nts.end());
    assert(sz == std::accumulate(nts.begin(), nts.end(), 0U));

    if (rank == 1) {
        return std::vector<size_t>{0U, lnt, biggest};
    }

    return std::vector<size_t>{
        std::accumulate(nts.begin(), nts.begin() + rank - 1U, 0U), lnt,
        biggest};
}

}  // namespace apps
}  // namespace exseis

#endif  // EXSEIS_PIOL_FOUR_D_SGLOBAL_HH
