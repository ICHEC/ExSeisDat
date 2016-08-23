#include "global.hh"
using namespace PIOL;

extern void mpiMakeSEGYCopy(Piol piol, std::string iname, std::string oname, size_t repRate);

std::pair<size_t, size_t> decompose(size_t work, size_t nproc, size_t rank)
{
    size_t q = work/nproc;
    size_t r = work%nproc;
    size_t start = q * rank + std::min(rank, r);
    return std::make_pair(start, std::min(work - start, q + (rank < r)));
}
std::pair<size_t, size_t> blockDecomp(size_t work, size_t bsz, size_t nproc, size_t rank)
{
    size_t bcnt = work / bsz + (work % bsz > 0);
    auto decomp = decompose(bcnt, nproc, rank);
    decomp.first *= bsz;
    decomp.second *= bsz;
    return decomp;
}
int main(int argc, char ** argv)
{
    Piol piol(new ExSeisPIOL);
//    mpiMakeSEGYCopy(Piol piol, "dat/big1.segy", 9);
//    mpiMakeSEGYCopy(Piol piol, "dat/big1.segy", 17);
//    mpiMakeSEGYCopy(Piol piol, "dat/big1.segy", 25);

    //mpiMakeSEGYCopy(piol, "dat/4D_REPEAT_LINES_2013_ALL_PRE-MIG_GATHERS.SEGY", "dat/big1.segy", 1);
    mpiMakeSEGYCopy(piol, "dat/GXT_IT5_RMS-TIME.segy", "dat/big1.segy", 1);
    return 0;
}

