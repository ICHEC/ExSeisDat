#include "sglobal.hh"
#include <memory>
#include <string>
#include <iostream>
#include <assert.h>
#include "cppfileapi.hh"
#include "share/segy.hh"
using namespace PIOL;

int main(int argc, char ** argv)
{
    std::string name = "";
    size_t ns = 0;
    size_t nt = 0;
    size_t max = 0;
    float inc = 0.0;

    //TODO document these arguments
    // o output file
    // s number of samples
    // t number of traces
    // m max
    // i increment
    std::string opt = "s:t:m:o:i:";  //TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'o' :
                name = optarg;
            break;
            case 's' :
                ns = std::stoul(optarg);
            break;
            case 't' :
                nt = std::stoul(optarg);
            break;
            case 'm' :
                max = std::stoul(optarg);
            break;
            case 'i' :
                inc = std::stod(optarg);
            break;
            default :
                fprintf(stderr, "One of the command line arguments is invalid\n");
            break;
        }
    assert(name.size() && ns && nt && max && inc != 0.0);
    max *= 1024 * 1024;

    ExSeis piol;
#warning delete
    std::cout << "name " << name << std::endl;
    File::Direct file(piol, name, FileMode::Write);
    piol.isErr();
    file.writeNs(ns);
    file.writeNt(nt);
    file.writeInc(inc);
    file.writeText("Test file\n");
    piol.isErr();

    auto dec = decompose(nt, piol.getNumRank(), piol.getRank());
    max /= std::max(SEGSz::getMDSz(), SEGSz::getDFSz(ns));

    //Not the optimal pattern
    for (size_t i = 0U; i < dec.second; i += max)
    {
        size_t rblock = (i + max < dec.second ? max : dec.second - i);
        {
            std::vector<File::TraceParam> prm(rblock);
            for (size_t j = 0; j < rblock; j++)
            {
                float k = dec.first+i+j;
                prm[j].src = {1600.0 + k, 2400.0 + k};
                prm[j].rcv = {100000.0 + k, 3000000.0 + k};
                prm[j].cmp = {10000.0 + k, 4000.0 + k};
                prm[j].line = {2400 + k, 1600 + k};
                prm[j].tn = dec.first+i+j;
            }
            file.writeTraceParam(dec.first + i, rblock, prm.data());
            piol.isErr();
        }
        {
            std::vector<float> trc(rblock*ns);
            for (size_t j = 0; j < trc.size(); j++)
                trc[j] = float((dec.first+i)*ns+j);
            file.writeTrace(dec.first + i, rblock, trc.data());
            piol.isErr();
        }
    }
    return 0;
}

