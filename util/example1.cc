#include <memory>
#include <string>
#include <iostream>
#include "global.hh"
#include "anc/piol.hh"
#include "file/filesegy.hh"
using namespace PIOL;
int main(void)
{
    //Initialise the PIOL by creating an ExSeisPIOL object
    auto piol = std::make_shared<ExSeisPIOL>();

    size_t rank = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();

    //Create a SEGY file object
    File::SEGY file(piol, "test.segy", FileMode::Write);

    //lnt is the number of traces and sets of trace parameters we will write per process
    size_t lnt = 40, ns = 300;
    double inc = 0.04;

    //Write some header parameters
    file.writeNs(ns);
    file.writeNt(lnt*numRank);
    file.writeInc(inc);
    file.writeText("Test file\n");

    //Set and write some trace parameters
    std::vector<File::TraceParam> prm(lnt);
    for (size_t j = 0; j < lnt; j++)
    {
        float k = lnt*rank+j;
        prm[j].src = {1600.0 + k, 2400.0 + k};
        prm[j].rcv = {100000.0 + k, 3000000.0 + k};
        prm[j].cmp = {10000.0 + k, 4000.0 + k};
        prm[j].line = {2400 + k, 1600 + k};
        prm[j].tn = lnt*rank+j;
    }
    file.writeTraceParam(lnt*rank, lnt, prm.data());

    //Set and write some traces
    std::vector<float> trc(lnt*ns);
    for (size_t j = 0; j < lnt*ns; j++)
        trc[j] = float(lnt*rank+j);
    file.writeTrace(lnt*rank, lnt, trc.data());
    return 0;
}

