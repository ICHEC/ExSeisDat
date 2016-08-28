#include "global.hh"
#include <assert.h>
#warning remove
#include <iostream>
#include <string>
using namespace PIOL;

extern void mpiMakeSEGYCopy(Piol piol, std::string iname, std::string oname, size_t repRate);
extern void mpiMakeSEGYCopyNaive1(Piol piol, std::string iname, std::string oname, size_t repRate);
extern void mpiMakeSEGYCopyNaive2(Piol piol, std::string iname, std::string oname, size_t repRate);

std::pair<size_t, size_t> decompose(size_t sz, size_t numRank, size_t rank)
{
    size_t q = sz/numRank;
    size_t r = sz%numRank;
    size_t start = q * rank + std::min(rank, r);
    return std::make_pair(start, std::min(sz - start, q + (rank < r)));
}
/*std::pair<size_t, size_t> blockDecomp(size_t sz, size_t bsz, size_t numRank, size_t rank)
{
    size_t bcnt = sz / bsz + (sz % bsz > 0);
    auto decomp = decompose(bcnt, numRank, rank);
    decomp.first *= bsz;
    decomp.second *= bsz;
    return decomp;
}*/
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

int main(int argc, char ** argv)
{
    assert(argc > 2);
    Piol piol(new ExSeisPIOL);
    int select = std::stoi(argv[1]);
    switch (select)
    {
        case 1 :
        if (!piol->comm->getRank())
            std::cout << "Standard\n";
        mpiMakeSEGYCopy(piol, argv[2], argv[3], std::stoi(argv[4]));
        break;
        case 2 :
        if (!piol->comm->getRank())
            std::cout << "Naive 1\n";
        mpiMakeSEGYCopyNaive1(piol, argv[2], argv[3], std::stoi(argv[4]));
        break;
        default :
        case 3 :
        if (!piol->comm->getRank())
            std::cout << "Naive 2\n";
        mpiMakeSEGYCopyNaive2(piol, argv[2], argv[3], std::stoi(argv[4]));
    }
    return 0;
}

