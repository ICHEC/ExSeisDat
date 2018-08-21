#include "sglobal.hh"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <numeric>

std::vector<size_t> lobdecompose(
  exseis::PIOL::ExSeisPIOL* piol, size_t sz, size_t numRank, size_t rank)
{
    double total = (numRank * (numRank + 1U)) / 2U;
    rank++;
    size_t lnt = std::lround(double(sz * rank) / total);

    auto rem = sz - piol->comm->sum(lnt);
    if (rank == 1) {
        lnt += rem;
    }

    auto nts       = piol->comm->gather<size_t>(lnt);
    size_t biggest = *std::max_element(nts.begin(), nts.end());
    assert(sz == std::accumulate(nts.begin(), nts.end(), 0U));

    if (rank == 1) {
        return std::vector<size_t>{0U, lnt, biggest};
    }
    else {
        return std::vector<size_t>{
          std::accumulate(nts.begin(), nts.begin() + rank - 1U, 0U), lnt,
          biggest};
    }
}

exseis::utils::Contiguous_decomposition blockDecomp(
  size_t sz, size_t bsz, size_t numRank, size_t rank, size_t off)
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
    auto newdec = exseis::utils::block_decomposition(bcnt, numRank, rank);

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
    if (
      newdec.local_size != 0
      && ((newdec.global_offset + newdec.local_size) > sz)) {
        newdec.local_size -= bsz - rend;
    }

    return newdec;
}
