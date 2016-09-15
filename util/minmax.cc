#include "sglobal.hh"
#include "ops/ops.hh"
#include "file/filesegy.hh"
#include <iostream>
using namespace PIOL;
using namespace File;
int calcMin(Piol piol, std::string iname, std::string oname)
{
    std::unique_ptr<Interface> in = std::make_unique<SEGY>(piol, iname);

    auto dec = decompose(in->readNt(), piol->comm->getNumRank(), piol->comm->getRank());
    size_t offset = dec.first;
    size_t num = dec.second;

    std::vector<coord_t> buf;
    try
    {
        buf.resize(num);
    }
    catch (std::bad_alloc e)
    {
        std::cout << "Could not allocate vector" << std::endl;
        return -1;
    }

    std::vector<CoordElem> minmax(12U);

    in->readCoordPoint(Coord::Src, offset, num, buf.data());
    getMinMax(piol, offset, num, buf.data(), minmax.data());

    in->readCoordPoint(Coord::Rcv, offset, num, buf.data());
    getMinMax(piol, offset, num, buf.data(), minmax.data()+4U);

    in->readCoordPoint(Coord::CMP, offset, num, buf.data());
    getMinMax(piol, offset, num, buf.data(), minmax.data()+8U);

    std::unique_ptr<Interface> out = std::make_unique<SEGY>(piol, oname, FileMode::Write);
    out->writeNt(minmax.size());
    out->writeNs(1U);
    out->writeInc(in->readInc());

    TraceParam hdr;
    for (size_t i = 0U; i < minmax.size(); i++)
    {
        in->readTraceParam(i, 1U, &hdr);
        hdr.tn = minmax[i].num;
        out->writeTraceParam(i, 1U, &hdr);
    }

    std::vector<trace_t> trace(minmax.size());
    for (auto & d : trace)
        d = trace_t(1);
    out->writeTrace(0, trace.size(), trace.data());
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

    Piol piol(new ExSeisPIOL);
    calcMin(piol, iname, oname);

    return 0;
}


