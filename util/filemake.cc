#include "sglobal.hh"
#include <assert.h>
#include <memory>
#include <string>
#include <functional>
#include <iostream>
#include <cmath>
#include "cppfileapi.hh"
#include "share/segy.hh"
using namespace PIOL;

void writeContig(ExSeis piol, File::Interface * file, size_t offset, size_t nt, size_t ns, size_t lnt, size_t extra, size_t max)
{
    float fhalf = float(nt*ns)/2.0;
    float off = float(nt*ns)/4.0;
    long nhalf = nt/2;
    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        std::vector<File::TraceParam> prm(rblock);
        for (size_t j = 0; j < rblock; j++)
        {
            float k = nhalf - std::abs(-nhalf + long(offset+i+j));
            prm[j].src = {1600.0 + k, 2400.0 + k};
            prm[j].rcv = {100000.0 + k, 3000000.0 + k};
            prm[j].cmp = {10000.0 + k, 4000.0 + k};
            prm[j].line = {2400 + k, 1600 + k};
            prm[j].tn = offset+i+j;
        }
        piol.isErr();
        std::vector<float> trc(rblock*ns);
        for (size_t j = 0; j < trc.size(); j++)
            trc[j] = fhalf - std::abs(-fhalf + float((offset+i)*ns+j)) - off;
        file->writeTrace(offset + i, rblock, trc.data(), prm.data());
        piol.isErr();
    }
    for (size_t j = 0; j < extra; j++)
        file->writeTrace(0U, size_t(0), nullptr, (File::TraceParam *)1);
}

void FileMake(bool lob, const std::string name, size_t max, size_t ns, size_t nt, geom_t inc)
{
    ExSeis piol;
    File::Direct file(piol, name, FileMode::Write);

    piol.isErr();
    file.writeNs(ns);
    file.writeNt(nt);
    file.writeInc(inc);
    file.writeText("Test file\n");
    piol.isErr();

    size_t offset = 0;
    size_t lnt = 0;
    size_t biggest = 0;

    if (lob)
    {
        auto dec = lobdecompose(piol, nt, piol.getNumRank(), piol.getRank());
        offset = dec[0];
        lnt = dec[1];
        biggest = dec[2];
    }
    else
    {
        auto dec = decompose(nt, piol.getNumRank(), piol.getRank());
        offset = dec.first;
        biggest = lnt = dec.second;
    }

    max /= SEGSz::getDOSz(ns);
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);
    writeContig(piol, &file, offset, nt, ns, lnt, extra, max);
}

int main(int argc, char ** argv)
{
    std::string name = "";
    size_t ns = 0;
    size_t nt = 0;
    size_t max = 0;
    geom_t inc = 0.0;
    bool lob = false;

    if (argc <= 1)
    {
        std::cout << "Options: filemake -o \"name.segy\" -s <ns> -t <nt> -m <mem(MiB)> -i <inc>\n";
        return EXIT_FAILURE;
    }
    //TODO document these arguments
    // o output file
    // s number of samples
    // t number of traces
    // m max
    // i increment
    std::string opt = "s:t:m:o:i:l";  //TODO: uses a GNU extension
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
            case 'l' :
                lob = true;
            break;
            default :
                std::cerr << "One of the command line arguments is invalid\n";
            break;
        }

    assert(name.size() && max && inc != 0.0);
    max *= 1024 * 1024;
    FileMake(lob, name, max, ns, nt, inc);
    return 0;
}

