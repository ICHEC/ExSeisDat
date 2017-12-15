#include <assert.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <fstream>
#include "cppfileapi.hh"
#include "sglobal.hh"
#include "ops/sort.hh"

using namespace PIOL;
using namespace File;

int main(int argc, char ** argv)
{
    ExSeis piol;

    std::string opt = "i:o:t:d";  //TODO: uses a GNU extension
    std::string name1 = "";
    auto type = PIOL_SORTTYPE_SrcRcv;
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

    File::ReadDirect src(piol.piol(), name1);

    //Perform the decomposition and read coordinates of interest.
    auto dec = decompose(piol.piol().get(), src);

    if (checkOrder(src, dec, type))
        std::cout << "Success\n";
    else
        std::cerr << "Failure\n";

    return 0;
}
