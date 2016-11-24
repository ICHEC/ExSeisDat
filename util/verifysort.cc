#include <assert.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <fstream>
#include "cppfileapi.hh"
#include "sglobal.hh"
#include "fileops.hh"

using namespace PIOL;
using namespace File;

int main(int argc, char ** argv)
{
    ExSeis piol;

    std::string opt = "i:o:t:d";  //TODO: uses a GNU extension
    std::string name1 = "";
    auto type = SortType::SrcRcv;
    bool debug = false;
    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'i' :
                name1 = optarg;
            break;
            case 't' :
                type = static_cast<SortType>(std::stoul(optarg));
            break;
            default :
                fprintf(stderr, "One of the command line arguments is invalid\n");
            break;
        }

    assert(name1.size());

    size_t rank = piol.getRank();
    size_t numRank = piol.getNumRank();

    File::Direct src(piol, name1, FileMode::Read);

    //Perform the decomposition and read coordinates of interest.
    auto dec = decompose(src.readNt(), numRank, rank);

    if (checkOrder(src, dec))
        std::cout << "Success\n";
    else
        std::cerr << "Failure\n";

    return 0;
}
