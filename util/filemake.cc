#include "sglobal.hh"
#include <assert.h>
#include <memory>
#include <string>
#include <iostream>
#include <cmath>
#include "cppfileapi.hh"
#include "share/segy.hh"
using namespace PIOL;
void FileMake(const std::string name, size_t max, size_t ns, size_t nt, geom_t inc)
{
    ExSeis piol;
    File::Direct file(piol, name, FileMode::Write);
    piol.isErr();
    file.writeNs(ns);
    file.writeNt(nt);
    file.writeInc(inc);
    file.writeText("Test file\n");
    piol.isErr();

    auto dec = decompose(nt, piol.getNumRank(), piol.getRank());
    max /= std::max(SEGSz::getMDSz(), SEGSz::getDFSz(ns));
    float fhalf = float(nt*ns)/2.0;
    float off = float(nt*ns)/4.0;
    long nhalf = nt/2;

    //Not the optimal pattern
    for (size_t i = 0U; i < dec.second; i += max)
    {
        size_t rblock = (i + max < dec.second ? max : dec.second - i);
        {
            std::vector<File::TraceParam> prm(rblock);
            for (size_t j = 0; j < rblock; j++)
            {
                float k = nhalf - std::abs(-nhalf + long(dec.first+i+j));
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
                trc[j] = fhalf - std::abs(-fhalf + float((dec.first+i)*ns+j)) - off;
            file.writeTrace(dec.first + i, rblock, trc.data());
            piol.isErr();
        }
    }
}

int main(int argc, char ** argv)
{
    std::string name = "";
    size_t ns = 0;
    size_t nt = 0;
    size_t max = 0;
    geom_t inc = 0.0;

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
                std::cerr << "One of the command line arguments is invalid\n";
            break;
        }

    assert(name.size() && max && inc != 0.0);
    max *= 1024 * 1024;

    FileMake(name, max, ns, nt, inc);
    return 0;
}

