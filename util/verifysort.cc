#include "sglobal.hh"

#include "ExSeisDat/PIOL/ExSeis.hh"
#include "ExSeisDat/PIOL/ReadDirect.hh"
#include "ExSeisDat/PIOL/operations/sort.hh"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unistd.h>

using namespace exseis::utils;
using namespace exseis::PIOL;

int main(int argc, char** argv)
{
    auto piol = ExSeis::New();

    std::string opt   = "i:o:t:d";  // TODO: uses a GNU extension
    std::string name1 = "";
    SortType type     = PIOL_SORTTYPE_SrcRcv;
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
        switch (c) {
            case 'i':
                name1 = optarg;
                break;

            case 't':
                type = static_cast<SortType>(std::stoul(optarg));
                break;

            default:
                fprintf(
                  stderr, "One of the command line arguments is invalid\n");
                break;
        }
    }

    assert(name1.size());

    ReadDirect src(piol, name1);

    // Perform the decomposition and read coordinates of interest.
    auto dec = block_decomposition(
      src.readNt(), piol->comm->getNumRank(), piol->comm->getRank());

    if (checkOrder(src, dec, type)) {
        std::cout << "Success\n";
    }
    else {
        std::cerr << "Failure\n";
    }

    return 0;
}
