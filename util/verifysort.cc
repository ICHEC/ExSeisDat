#include "sglobal.hh"

#include "exseisdat/piol/ExSeis.hh"
#include "exseisdat/piol/ReadSEGY.hh"
#include "exseisdat/piol/operations/sort.hh"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unistd.h>

using namespace exseis::utils;
using namespace exseis::piol;

int main(int argc, char** argv)
{
    auto piol = ExSeis::make();

    std::string opt = "i:o:t:d";  // TODO: uses a GNU extension
    std::string name1;
    SortType type = SortType::SrcRcv;
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

    assert(!name1.empty());

    ReadSEGY src(piol, name1);

    // Perform the decomposition and read coordinates of interest.
    auto dec = block_decomposition(
      src.read_nt(), piol->comm->get_num_rank(), piol->comm->get_rank());

    if (check_order(src, dec, type)) {
        std::cout << "Success\n";
    }
    else {
        std::cerr << "Failure\n";
    }

    return 0;
}
