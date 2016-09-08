#include "sglobal.hh"
#include <assert.h>
std::pair<size_t, size_t> decompose(size_t sz, size_t numRank, size_t rank)
{
    assert(numRank > rank);
    size_t q = sz/numRank;
    size_t r = sz%numRank;
    size_t start = q * rank + std::min(rank, r);
    return std::make_pair(start, std::min(sz - start, q + (rank < r)));
}

std::pair<size_t, size_t> blockDecomp(size_t sz, size_t bsz, size_t numRank, size_t rank, size_t off)
{
    size_t rstart = bsz - off % bsz;                //Size of the first block
    size_t rend = (off + sz) % bsz;                 //Size of the last block
    size_t bcnt = (sz - rstart - rend) / bsz + 2;   //The total number of blocks, +2 accounts for start+end

    assert(!((sz - rstart - rend) % bsz));          //Make sure there isn't any leftover, this shouldn't be possible

    auto newdec = decompose(bcnt, numRank, rank);   //Do a regular decomposition of the blocks

    newdec.first *= bsz;
    newdec.second *= bsz;
    if (newdec.second == 0)
        return {sz, 0};

    //Now we compensate for the fact that the start and end block sizes can be different.
    if (!rank)                              //If the rank is zero, we shrink the first block by the leftover
        newdec.second -= bsz - rstart;
    else                                    //The subtraction above means every block is shifted
        newdec.first -= bsz - rstart;

    if (newdec.second && newdec.first+newdec.second > sz)    //The last rank with work must remove the leftover of its last block
        newdec.second -= bsz - rend;
    return newdec;
}

