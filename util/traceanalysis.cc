////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date March 2017
/// @brief
/// @details Perform an analysis of a single trace.
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL.hh"

#include <iostream>

#include <unistd.h>  //getopt

using namespace PIOL;

/*! Main function for traceanalysis.
 *  @param[in] argc The number of input strings.
 *  @param[in] argv The array of input strings.
 *  @return zero on success, non-zero on failure
 *  @details Two command line options:
 *           -i \<inp\> : input file
 *           -t \<num\> : input trace number (default 0)
 */
int main(int argc, char** argv)
{
    auto piol = ExSeis::New();

    std::string name = "";
    size_t tn        = 0LU;
    std::string opt  = "i:t:";  // TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str()))
        switch (c) {
            case 'i':
                name = optarg;
                break;
            case 't':
                tn = std::stoul(optarg);
                break;
            default:
                std::cerr << "One of the command line arguments is invalid\n";
                break;
        }
    File::ReadDirect file(piol, name);

    File::Param prm(1LU);
    file->readParam(tn, 1LU, &prm);

    if (!piol->getRank()) {
        std::cout << "xSrc " << File::getPrm<geom_t>(0LU, PIOL_META_xSrc, &prm)
                  << std::endl;
        std::cout << "ySrc " << File::getPrm<geom_t>(0LU, PIOL_META_ySrc, &prm)
                  << std::endl;
        std::cout << "xRcv " << File::getPrm<geom_t>(0LU, PIOL_META_xRcv, &prm)
                  << std::endl;
        std::cout << "yRcv " << File::getPrm<geom_t>(0LU, PIOL_META_yRcv, &prm)
                  << std::endl;
        std::cout << "xCmp " << File::getPrm<geom_t>(0LU, PIOL_META_xCmp, &prm)
                  << std::endl;
        std::cout << "yCmp " << File::getPrm<geom_t>(0LU, PIOL_META_yCmp, &prm)
                  << std::endl;

        std::cout << "il " << File::getPrm<size_t>(0LU, PIOL_META_il, &prm)
                  << std::endl;
        std::cout << "xl " << File::getPrm<size_t>(0LU, PIOL_META_xl, &prm)
                  << std::endl;
    }
    return 0;
}
