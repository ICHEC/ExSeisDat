////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date March 2017
/// @brief
/// @details Perform an analysis of a single trace.
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/configuration/ExSeis.hh"
#include "exseisdat/piol/file/Input_file_segy.hh"

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

    Input_file_segy file(piol, name);

    Trace_metadata trace_metadata(1LU);
    file.read_metadata(tn, 1LU, trace_metadata);

    if (piol->get_rank() == 0) {
        std::cout << "x_src "
                  << trace_metadata.get_floating_point(
                         0LU, Trace_metadata_key::x_src)
                  << std::endl;
        std::cout << "y_src "
                  << trace_metadata.get_floating_point(
                         0LU, Trace_metadata_key::y_src)
                  << std::endl;
        std::cout << "x_rcv "
                  << trace_metadata.get_floating_point(
                         0LU, Trace_metadata_key::x_rcv)
                  << std::endl;
        std::cout << "y_rcv "
                  << trace_metadata.get_floating_point(
                         0LU, Trace_metadata_key::y_rcv)
                  << std::endl;
        std::cout << "xCmp "
                  << trace_metadata.get_floating_point(
                         0LU, Trace_metadata_key::xCmp)
                  << std::endl;
        std::cout << "yCmp "
                  << trace_metadata.get_floating_point(
                         0LU, Trace_metadata_key::yCmp)
                  << std::endl;

        std::cout << "il "
                  << trace_metadata.get_integer(0LU, Trace_metadata_key::il)
                  << std::endl;
        std::cout << "xl "
                  << trace_metadata.get_integer(0LU, Trace_metadata_key::xl)
                  << std::endl;
    }
    return 0;
}
