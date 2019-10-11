////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date November 2016
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "4dcore.hh"
#include "4dio.hh"

#include "exseisdat/piol/configuration/ExSeis.hh"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iostream>
#include <numeric>
#include <stdlib.h>
#include <unistd.h>

using namespace exseis::piol;
using namespace exseis::piol::four_d;


/*! Main function for fourdbin.
 *  @param[in] argc The number of input strings.
 *  @param[in] argv The array of input strings.
 *  @return zero on success, non-zero on failure
 *  @details 4 files must be specified on the command line:
 *           -a \<inp1\> : First input file
 *           -b \<inp2\> : Second input file
 *           -c \<out1\> : First output file
 *           -d \<out2\> : Second output file
 *           -t \<val\> : floating value for dsrmax
 *           -v : Use this option for extra verbosity
 *  @return Return zero on success, non-zero on failure.
 */
int main(int argc, char** argv)
{
    auto piol = ExSeis::make();

    fourd_t dsrmax = 1.0;  // Default dsdr criteria
    std::string name1;
    std::string name2;
    std::string name3;
    std::string name4;
    Four_d_opt fopt;

    char mpi_version[MPI_MAX_LIBRARY_VERSION_STRING - 1];
    int len;
    MPI_Get_library_version(mpi_version, &len);
    if (piol->get_rank() == 0) {
        std::cout << "MPI Version " << mpi_version << std::endl;
    }

    /*****************  Reading options from the command line *****************/
    std::string opt = "a:b:c:d:t:vpx";  // TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
        switch (c) {
            case 'a':
                name1 = optarg;
                break;

            case 'b':
                name2 = optarg;
                break;

            case 'c':
                name3 = optarg;
                break;

            case 'd':
                name4 = optarg;
                break;

            case 't':
                dsrmax = std::stod(optarg);
                break;

            case 'v':
                fopt.verbose = true;
                cmsg(piol.get(), "Verbose mode enabled");
                break;

            case 'p':
                fopt.print_dsr = false;
                break;

            case 'x':
                fopt.ixline = true;
                break;

            default:
                std::cerr << "One of the command line arguments is invalid\n";
                break;
        }
    }
    assert(
        !name1.empty() && !name2.empty() && !name3.empty() && !name4.empty());

    /**************************************************************************/

    // Open the two input files
    cmsg(piol.get(), "Parameter-read phase");

    // Perform the decomposition and read the coordinates of interest.
    auto coords1 = get_coords(piol, name1, fopt.ixline);
    auto coords2 = get_coords(piol, name2, fopt.ixline);

    std::vector<size_t> min(coords1->sz);
    std::vector<fourd_t> minrs(coords1->sz);
    calc_4d_bin(
        piol.get(), dsrmax, coords1.get(), coords2.get(), fopt, min, minrs);
    coords2.release();

    cmsg(piol.get(), "Final list pass");
    // Now we weed out traces that have a match that is too far away
    std::vector<size_t> list1;
    std::vector<size_t> list2;
    std::vector<fourd_t> lminrs;

    for (size_t i = 0U; i < coords1->sz; i++) {
        if (minrs[i] <= dsrmax) {
            list2.push_back(min[i]);
            lminrs.push_back(minrs[i]);
            list1.push_back(coords1->tn[i]);
        }
    }

    if (fopt.verbose) {
        std::string name = "tmp/restart" + std::to_string(piol->get_rank());
        FILE* f_out      = fopen(name.c_str(), "w+");
        size_t sz        = list1.size();
        assert(fwrite(&sz, sizeof(size_t), 1U, f_out) == 1U);
        assert(fwrite(list1.data(), sizeof(size_t), sz, f_out) == sz);
        assert(fwrite(list2.data(), sizeof(size_t), sz, f_out) == sz);
        assert(fwrite(lminrs.data(), sizeof(fourd_t), sz, f_out) == sz);
        fclose(f_out);
    }

    // free up some memory
    coords1.release();

    cmsg(piol.get(), "Output phase");

    output_non_mono(piol, name3, name1, list1, lminrs, fopt.print_dsr);
    output_non_mono(piol, name4, name2, list2, lminrs, fopt.print_dsr);

    return 0;
}
