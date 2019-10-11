///
/// Example: minmax.cc    {#examples_minmax_cc}
/// ==================
///
/// @todo DOCUMENT ME - Finish documenting example.
///
/// This is an example of using minmax through the Set API is complete.
///

#include "exseisdat/flow.hh"

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <unistd.h>

using namespace exseis::piol;
using namespace exseis::flow;

int main(int argc, char** argv)
{
    auto piol = ExSeis::make();

    std::string opt = "i:";  // TODO: uses a GNU extension
    std::string iname;
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
        if (c == 'i') {
            iname = optarg;
        }
        else {
            std::cerr << "One of the command line arguments is invalid.\n";
        }
    }
    assert(!iname.empty());

    Set set(piol, iname);

    CoordElem minmax[12];
    set.get_min_max(
        Trace_metadata_key::x_src, Trace_metadata_key::y_src, &minmax[0]);
    set.get_min_max(
        Trace_metadata_key::x_rcv, Trace_metadata_key::y_rcv, &minmax[4]);
    set.get_min_max(
        Trace_metadata_key::xCmp, Trace_metadata_key::yCmp, &minmax[8]);

    if (piol->get_rank() == 0) {
        std::cout << "x Src " << minmax[0].val << " (" << minmax[0].num
                  << ") -> " << minmax[1].val << " (" << minmax[1].num << ")\n";
        std::cout << "y Src " << minmax[2].val << " (" << minmax[2].num
                  << ") -> " << minmax[3].val << " (" << minmax[3].num << ")\n";
        std::cout << "x Rcv " << minmax[4].val << " (" << minmax[4].num
                  << ") -> " << minmax[5].val << " (" << minmax[5].num << ")\n";
        std::cout << "y Rcv " << minmax[6].val << " (" << minmax[6].num
                  << ") -> " << minmax[7].val << " (" << minmax[7].num << ")\n";
        std::cout << "x Cmp " << minmax[8].val << " (" << minmax[8].num
                  << ") -> " << minmax[9].val << " (" << minmax[9].num << ")\n";
        std::cout << "y Cmp " << minmax[10].val << " (" << minmax[10].num
                  << ") -> " << minmax[11].val << " (" << minmax[11].num
                  << ")\n";
    }
    return 0;
}
