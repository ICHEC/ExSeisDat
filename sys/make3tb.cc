#include "global.hh"
#include "data/datampiio.hh"
#include <iostream>
using namespace PIOL;
using namespace Data;

#warning EVENTUALLY WRITE AN OBJECT LAYER TEST
std::pair<size_t, size_t> decompose(size_t work, size_t nproc, size_t rank)
{
    size_t q = work/nproc;
    size_t r = work%nproc;
    size_t start = q * rank + std::min(rank, r);
    return std::make_pair(start, std::min(work - start, q + (rank < r)));
}

int main(int argc, char ** argv)
{
    Piol piol(new ExSeisPIOL);
    MPIIOOpt opt;
    MPIIO in(piol, argv[1], opt);

    opt.mode = MPI_MODE_CREATE | MPI_MODE_WRONLY;
    MPIIO out(piol, "dat/bigsegy.segy", opt);

    size_t fsz = in.getFileSz();
//    out.setFileSz(fsz*10U - 3600U*9U);

    if (!piol->comm->getRank())
    {
        std::vector<uchar> header(3600U);
        in.read(0U, header.size(), header.data());
        out.write(0U, header.size(), header.data());
    }

    auto piece = decompose(fsz-3600U, piol->comm->getNumRank(), piol->comm->getRank());

    size_t pieceSz = 2U*1024U*1024U*1024U;
    std::vector<uchar> buf(pieceSz);
    size_t q = piece.second / pieceSz;
    size_t r = piece.second % pieceSz;
    for (size_t i = 0; i < q; i++)
    {
        size_t offset = 3600U + piece.first + pieceSz*i;
        in.read(offset, buf.size(), buf.data());

//        out.write(offset, buf.size(), buf.data());
        for (size_t j = 0; j < 10; j++)
            out.write(offset + (fsz-3600U) * j, buf.size(), buf.data());
    }
    size_t offset = 3600U + piece.first + pieceSz*q;
    in.read(offset, r, buf.data());
    for (size_t j = 0; j < 10; j++)
        out.write(offset + (fsz-3600U) * j, r, buf.data());

/*    auto piece = decompose(fsz-3600U, piol->comm->getNumRank(), piol->comm->getRank());

    std::vector<uchar> buf(piece.second);
    in.read(3600U + piece.first, buf.size(), buf.data());

    out.write(3600U + piece.first, buf.size(), buf.data());
    for (size_t i = 0; i < 9; i++)
        out.write(fsz + (fsz-3600U) * i + piece.first, buf.size(), buf.data());*/
    return -1;
}

