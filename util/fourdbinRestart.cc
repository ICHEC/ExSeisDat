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
    std::string name1 = "";
    std::string name2 = "";
    std::string name3 = "";
    std::string name4 = "";
    FourDOpt fopt;

    char MPIVersion[MPI_MAX_LIBRARY_VERSION_STRING-1];
    int len;
    MPI_Get_library_version(MPIVersion, &len);
    if (!piol.getRank())
        std::cout << "MPI Version " << MPIVersion << std::endl;

/*******************  Reading options from the command line ***********************************************/
    std::string opt = "a:b:c:d:t:vpx";  //TODO: uses a GNU extension
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
            break;
            case 'v' :
                fopt.verbose = true;
                cmsg(piol, "Verbose mode enabled");
            break;
            case 'p' :
                fopt.printDsr = false;
            break;
            case 'x' :
                fopt.ixline = true;
            break;
            default :
                std::cerr<< "One of the command line arguments is invalid\n";
            break;
        }
    assert(name1.size() && name2.size() && name3.size() && name4.size());
/**********************************************************************************************************/
    std::vector<size_t> list1;
    std::vector<size_t> list2;
    std::vector<fourd_t> lminrs;

    std::string name = "tmp/restart" + std::to_string(piol.getRank());
    FILE * fOut = fopen(name.c_str(), "r+");
    size_t sz;
    assert(fread(&sz, sizeof(size_t), 1U, fOut) == 1U);
    list1.resize(sz);
    list2.resize(sz);
    lminrs.resize(sz);
    assert(fread(list1.data(), sizeof(size_t), sz, fOut) == sz);
    assert(fread(list2.data(), sizeof(size_t), sz, fOut) == sz);
    assert(fread(lminrs.data(), sizeof(fourd_t), sz, fOut) == sz);
    fclose(fOut);

    outputNonMono(piol, name3, name1, list1, lminrs, fopt.printDsr);
    outputNonMono(piol, name4, name2, list2, lminrs, fopt.printDsr);

    return 0;
}
