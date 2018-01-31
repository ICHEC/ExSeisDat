/*******************************************************************************************//*!
 *   @file
 *   @author Cathal O Broin - cathal@ichec.ie - first commit
 *   @copyright TBD. Do not distribute
 *   @date February 2017
 *   @brief This is an example of using minmax through the Set API
 *   is complete.
*//*******************************************************************************************/
#include "flow.hh"

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <unistd.h>

using namespace PIOL;

int main(int argc, char** argv)
{
    auto piol = ExSeis::New();

    std::string opt   = "i:";  //TODO: uses a GNU extension
    std::string iname = "";
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str()))
        if (c == 'i')
            iname = optarg;
        else
            std::cerr << "One of the command line arguments is invalid.\n";
    assert(iname != "");

    Set set(piol, iname);

    CoordElem minmax[12];
    set.getMinMax(PIOL_META_xSrc, PIOL_META_ySrc, &minmax[0]);
    set.getMinMax(PIOL_META_xRcv, PIOL_META_yRcv, &minmax[4]);
    set.getMinMax(PIOL_META_xCmp, PIOL_META_yCmp, &minmax[8]);

    if (!piol->getRank()) {
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
