/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \date November 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "4dio.hh"
#include "4dcore.hh"

using namespace PIOL;
using namespace FOURD;

namespace PIOL {
    void cmsg(ExSeisPIOL * piol, std::string msg)
    {
        piol->comm->barrier();
        if (!piol->comm->getRank())
            std::cout << msg << std::endl;
    }
}

/*! Main function for fourdbin.
 *  \param[in] argc The number of input strings.
 *  \param[in] argv The array of input strings.
 *  \return zero on success, non-zero on failure
 *  \details 4 files must be specified on the command line:
 *           -a \<inp1\> : First input file
 *           -b \<inp2\> : Second input file
 *           -c \<out1\> : First output file
 *           -d \<out2\> : Second output file
 *           -t \<val\> : floating value for dsrmax
 *           -v : Use this option for extra verbosity
 *  \return Return zero on success, non-zero on failure.
 */
int main(int argc, char ** argv)
{
    ExSeis piol;
    fourd_t dsrmax = 1.0;            //Default dsdr criteria
    bool verbose = false;
    std::string name1 = "";
    std::string name2 = "";
    std::string name3 = "";
    std::string name4 = "";

/*******************  Reading options from the command line ***********************************************/
    std::string opt = "a:b:c:d:t:v";  //TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'a' :
                name1 = optarg;
            break;
            case 'b' :
                name2 = optarg;
            break;
            case 'c' :
                name3 = optarg;
            break;
            case 'd' :
                name4 = optarg;
            break;
            case 't' :
                dsrmax = std::stod(optarg);
            break;
            case 'v' :
                verbose = true;
                cmsg(piol, "Verbose mode enabled");
            break;
            default :
                std::cerr<< "One of the command line arguments is invalid\n";
            break;
        }
    assert(name1.size() && name2.size() && name3.size() && name4.size());
/**********************************************************************************************************/
    //Open the two input files
    cmsg(piol, "Parameter-read phase");

    //Perform the decomposition and read the coordinates of interest.
    auto coords1 = getCoords(piol, name1);
    auto coords2 = getCoords(piol, name2);

    vec<size_t> min(coords1->sz);
    vec<fourd_t> minrs(coords1->sz);
    calc4DBin(piol, dsrmax, coords1.get(), coords2.get(), min, minrs, verbose);
    coords2.release();

    cmsg(piol, "Final list pass");
    //Now we weed out traces that have a match that is too far away
    vec<size_t> list1;
    vec<size_t> list2;
    vec<fourd_t> lminrs;

    for (size_t i = 0U; i < coords1->sz; i++)
        if (minrs[i] <= dsrmax)
        {
            list2.push_back(min[i]);
            lminrs.push_back(minrs[i]);
            list1.push_back(coords1->tn[i]);
        }
    //free up some memory
    coords1.release();

    cmsg(piol, "Output phase");

    outputNonMono(piol, name3, name1, list1, lminrs);
    outputNonMono(piol, name4, name2, list2, lminrs);

    return 0;
}
