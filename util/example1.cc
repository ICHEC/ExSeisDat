#include <memory>
#include <string>
#include <iostream>
#include "global.hh"
#include "anc/piol.hh"
#include "file/filesegy.hh"
using namespace PIOL;
int main(void)
{
    auto piol = std::make_shared<ExSeisPIOL>();
    size_t rank = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();

    auto file = std::make_unique<File::SEGY>(piol, "test.segy", FileMode::Write);
    piol->isErr();

    size_t lnt = 40, ns = 300;
    double inc = 0.04;
    file->writeNs(ns);
    file->writeNt(lnt*numRank);
    file->writeInc(inc);
    file->writeText("Test file\n");
    piol->isErr();

    std::vector<File::TraceParam> prm(lnt);
    for (size_t j = 0; j < lnt; j++)
    {
        size_t k = lnt*rank+j;
        prm[j].src = {1600.0 + float(k), 2400.0 + float(k)};
        prm[j].rcv = {100000.0 + float(k), 3000000.0 + float(k)};
        prm[j].cmp = {10000.0 + float(k), 4000.0 + float(k)};
        prm[j].line = {2400 + float(k), 1600 + float(k)};
        prm[j].tn = k;
    }
    file->writeTraceParam(lnt*rank, lnt, prm.data());
    piol->isErr();

    std::vector<float> trc(lnt*ns);
    for (size_t j = 0; j < lnt*ns; j++)
        trc[j] = float(lnt*rank+j);
    file->writeTrace(lnt*rank, lnt, trc.data());
    piol->isErr();
    return 0;
}

