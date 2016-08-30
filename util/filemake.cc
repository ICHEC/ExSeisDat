#include <memory>
#include <string>
#include <iostream>
#include "global.hh"
#include "anc/piol.hh"
#include "file/filesegy.hh"
#include "object/objsegy.hh"
#include "data/datampiio.hh"
#include "share/segy.hh"
using namespace PIOL;

std::pair<size_t, size_t> decompose(size_t sz, size_t numRank, size_t rank)
{
    size_t q = sz/numRank;
    size_t r = sz%numRank;
    size_t start = q * rank + std::min(rank, r);
    return std::make_pair(start, std::min(sz - start, q + (rank < r)));
}

int main(int argc, char ** argv)
{
    std::string outname(argv[1]);
    size_t ns = std::stoul(argv[2]);
    size_t nt = std::stoul(argv[3]);
    size_t max = std::stoul(argv[4]);

    float inc = 0.04;

    //TODO: This is an annoying way to create the PIOL object
    auto piol = std::make_shared<ExSeisPIOL>();
    size_t rank = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();

    if (!rank)
        std::cout << outname << " ns " << ns << " nt " << nt << " max " << max << std::endl;

    File::SEGYOpt segyOpt;
    Obj::SEGYOpt objOpt;
    Data::MPIIOOpt dataOpt;
    dataOpt.mode = FileMode::ReadWrite;

    if (!rank)
        std::cout << "Create file\n";
    //TODO:: Too many arguments
    auto file = std::make_unique<File::SEGY>(piol, outname, segyOpt, objOpt, dataOpt);
    piol->isErr();
    file->writeNs(ns);
    file->writeNt(nt);
    file->writeInc(inc);
    file->writeText("Test file\n");
    piol->isErr();

    auto dec = decompose(nt, numRank, rank);
    if (!rank)
        std::cout << "Max\n";
    max /= std::max(SEGSz::getMDSz(), SEGSz::getDFSz(ns));
    if (!rank)
        std::cout << "Post Max\n";
    for (size_t i = 0U; i < dec.second; i += max)
    {
        size_t rblock = (i + max < dec.second ? max : dec.second - i);
        {
            if (!rank)
                std::cout << "Write trace params\n";
            std::vector<File::TraceParam> prm(rblock);
            for (size_t j = 0; j < rblock; j++)
            {
                size_t k = dec.first+i+j;
                prm[j].src = {1600.0 + float(k), 2400.0 + float(k)};
                prm[j].rcv = {100000.0 + float(k), 3000000.0 + float(k)};
                prm[j].cmp = {10000.0 + float(k), 4000.0 + float(k)};
                prm[j].line = {2400 + float(k), 1600 + float(k)};
                prm[j].tn = k;
            }
            file->writeTraceParam(dec.first + i, dec.second, prm.data());
            piol->isErr();
        }
        {
            std::vector<float> trc(rblock*ns);
            for (size_t j = 0; j < rblock*ns; j++)
                trc[j] = float(dec.first+i+j);
            if (!rank)
                std::cout << "Write traces\n";
            file->writeTrace(dec.first + i, dec.second, trc.data());
            piol->isErr();
        }
    }
    return 0;
}

