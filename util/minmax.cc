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

    in->readCoordPoint(Coord::Src, offset, num, buf.data());
    geom_t min = xmin(piol, offset, num, buf.data());

    std::vector<CoordElem> minmax(4);
    getMinMax(piol, offset, num, buf.data(), minmax.data());

    std::cout << "xmin " << minmax[0].val << " tn " << minmax[0].num << "\n";
    std::cout << "xmax " << minmax[1].val << " tn " << minmax[1].num << "\n";
    std::cout << "ymin " << minmax[2].val << " tn " << minmax[2].num << "\n";
    std::cout << "ymax " << minmax[3].val << " tn " << minmax[3].num << "\n";

    std::unique_ptr<Interface> out = std::make_unique<SEGY>(piol, oname, FileMode::Write);
    out->writeNt(4);
    out->writeNs(1);
    out->writeInc(in->readInc());

    TraceParam hdr;
    trace_t data[] = { 1.0,  1.0, 1.0, 1.0 };
    for (size_t i = 0; i < minmax.size(); i++)
    {
        in->readTraceParam(i, 1, &hdr);
        hdr.tn = minmax[i].num;
        out->writeTraceParam(i, 1, &hdr);
    }
    out->writeTrace(0, 4, data);

    if (!piol->comm->getRank())
        std::cout << "Minimum x src for first " << num*piol->comm->getNumRank() << " traces is " << min << std::endl;
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


