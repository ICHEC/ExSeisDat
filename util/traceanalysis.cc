////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date March 2017
/// @brief
/// @details Perform an analysis of a single trace.
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/ExSeis.hh"
#include "exseisdat/piol/ReadSEGY.hh"

#include <iostream>
#include <unistd.h>

using namespace exseis::piol;

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
    auto piol = ExSeis::make();

    std::string name;
    size_t tn       = 0LU;
    std::string opt = "i:t:";  // TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
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
    }

    ReadSEGY file(piol, name);

    Trace_metadata prm(1LU);
    file.read_param(tn, 1LU, &prm);

    if (piol->get_rank() == 0) {
        std::cout << "x_src " << prm.get_floating_point(0LU, Meta::x_src)
                  << std::endl;
        std::cout << "y_src " << prm.get_floating_point(0LU, Meta::y_src)
                  << std::endl;
        std::cout << "x_rcv " << prm.get_floating_point(0LU, Meta::x_rcv)
                  << std::endl;
        std::cout << "y_rcv " << prm.get_floating_point(0LU, Meta::y_rcv)
                  << std::endl;
        std::cout << "xCmp " << prm.get_floating_point(0LU, Meta::xCmp)
                  << std::endl;
        std::cout << "yCmp " << prm.get_floating_point(0LU, Meta::yCmp)
                  << std::endl;

        std::cout << "il " << prm.get_integer(0LU, Meta::il) << std::endl;
        std::cout << "xl " << prm.get_integer(0LU, Meta::xl) << std::endl;
    }
    return 0;
}
