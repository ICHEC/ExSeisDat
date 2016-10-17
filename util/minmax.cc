#include "sglobal.hh"
#include "ops/ops.hh"
#include "cppfileapi.hh"
#include <iostream>
#include <algorithm>
using namespace PIOL;
using namespace File;
int calcMin(std::string iname, std::string oname)
{
    ExSeis piol;
    File::Direct in(piol, iname);

    auto dec = decompose(in.readNt(), piol.getNumRank(), piol.getRank());
    size_t offset = dec.first;
    size_t num = dec.second;

    for (size_t i = 0; i < piol.getNumRank(); i++)
    {
        if (i == piol.getRank())
            std::cout << piol.getRank() << " " << offset << " "  << num << std::endl;
        piol.barrier();
    }

    std::vector<TraceParam> prm;
    try
    {
        prm.resize(num);
    }
    catch (std::bad_alloc e)
    {
        std::cout << "Could not allocate vector" << std::endl;
        return -1;
    }

    std::vector<CoordElem> minmax(12U);
    in.readTraceParam(offset, num, prm.data());

    getMinMax(piol, offset, num, Coord::Src, prm.data(), minmax.data());
    getMinMax(piol, offset, num, Coord::Rcv, prm.data(), minmax.data()+4U);
    getMinMax(piol, offset, num, Coord::CMP, prm.data(), minmax.data()+8U);

    size_t sz = (!piol.getRank() ? minmax.size() : 0U);
    std::vector<size_t> list(sz);
    std::vector<size_t> uniqlist(sz);

    for (size_t i = 0U; i < sz; i++)
        uniqlist[i] = list[i] = minmax[i].num;

    std::sort(uniqlist.begin(), uniqlist.end());
    auto end = std::unique(uniqlist.begin(), uniqlist.end());
    uniqlist.resize(std::distance(uniqlist.begin(), end));

    size_t usz = uniqlist.size();

    std::vector<TraceParam> tprm(usz);
    in.readTraceParam(usz, uniqlist.data(), tprm.data());

    std::vector<TraceParam> oprm(sz);
    std::vector<trace_t> trace(sz);
    for (size_t i = 0U; i < sz; i++)
        for (size_t j = 0U; j < usz; j++)
        {
            if (list[i] == uniqlist[j])
            {
                oprm[i] = tprm[j];
                oprm[i].tn = minmax[i].num;
                trace[i] = trace_t(1);
                j = usz;
            }
        }

    File::Direct out(piol, oname, FileMode::Write);
    out.writeNt(sz);
    out.writeNs(1U);
    out.writeInc(in.readInc());
    out.writeTrace(0, sz, trace.data(), oprm.data());
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


