#include "sglobal.hh"
#include "cppfileapi.hh"
#include "fileops.hh"
#include <iostream>
#include <algorithm>
using namespace PIOL;
using namespace File;
int calcMin(std::string iname, std::string oname)
{
    ExSeis piol;
    File::ReadDirect in(piol, iname);

    auto dec = decompose(in.readNt(), piol.getNumRank(), piol.getRank());
    size_t offset = dec.first;
    size_t lnt = dec.second;

    Param prm(lnt);
    std::vector<CoordElem> minmax(12U);
    in.readParam(offset, lnt, &prm);

    getMinMax(piol, offset, lnt, Meta::xSrc, Meta::ySrc, &prm, minmax.data());
    getMinMax(piol, offset, lnt, Meta::xRcv, Meta::yRcv, &prm, minmax.data()+4U);
    getMinMax(piol, offset, lnt, Meta::xCmp, Meta::yCmp, &prm, minmax.data()+8U);

    size_t sz = (!piol.getRank() ? minmax.size() : 0U);
    size_t usz = 0;
    std::vector<size_t> list(sz);
    std::vector<size_t> uniqlist(sz);

    if (sz)
    {
        for (size_t i = 0U; i < sz; i++)
            uniqlist[i] = list[i] = minmax[i].num;

        std::sort(uniqlist.begin(), uniqlist.end());
        auto end = std::unique(uniqlist.begin(), uniqlist.end());
        uniqlist.resize(std::distance(uniqlist.begin(), end));

        usz = uniqlist.size();
    }

    Param tprm(usz);
    in.readParam(usz, uniqlist.data(), &tprm);

    Param oprm(sz);
    std::vector<trace_t> trace(sz);
    for (size_t i = 0U; i < sz; i++)
        for (size_t j = 0U; j < usz; j++)
        {
            if (list[i] == uniqlist[j])
            {
                cpyPrm(j, &tprm, i, &oprm);
                setPrm(i, Meta::tn,  minmax[i].num, &oprm);
                trace[i] = trace_t(1);
                j = usz;
            }
        }

    File::WriteDirect out(piol, oname);
    out.writeNt(sz);
    out.writeNs(1U);
    out.writeInc(in.readInc());
    out.writeTrace(0, sz, trace.data(), &oprm);
    return 0;
}

const std::string Arg = "Arguments: -i for input file, -o for output file\n";

int main(int argc, char ** argv)
{
    if (argc < 5)
    {
        std::cerr << Arg;
        return -1;
    }

// -i input file name
// -o output file name
    std::string opt = "i:o:";  //TODO: uses a GNU extension
    std::string iname = "";
    std::string oname = "";
    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'i' :
                iname = optarg;
            break;
            case 'o' :
                oname = optarg;
            break;
            default :
                std::cerr << "One of the command line arguments is invalid.\n";
            return -1;
        }

    if (iname == "" || oname == "")
    {
        std::cerr << "Invalid arguments given.\n" << Arg;
        return -1;
    }

    calcMin(iname, oname);

    return 0;
}


