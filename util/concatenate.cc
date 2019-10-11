////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date Q4 2016
/// @brief
/// @details This function takes one or more files as input and produces a new
///          file or files which contain all traces with identical ns and
///          increment.
////////////////////////////////////////////////////////////////////////////////

#include "sglobal.hh"

#include "exseisdat/flow/Set.hh"
#include "exseisdat/piol/configuration/ExSeis.hh"

#include <assert.h>
#include <iostream>
#include <unistd.h>

using namespace exseis::piol;
using namespace exseis::flow;

/*! Prompt the user asking them if they want to continue with concatenation.
 *  Multi-process safe.
 *  @param[in] piol The piol object.
 */
void do_prompt(ExSeisPIOL* piol)
{
    char cont   = '0';
    size_t rank = piol->comm->get_rank();
    if (rank == 0) {
        std::cout << "Continue concatenation? (y/n)\n";
        std::cin >> cont;
    }
    int err = MPI_Bcast(&cont, 1U, MPI_CHAR, 0, MPI_COMM_WORLD);
    if (err != MPI_SUCCESS) {
        std::cerr << "Scatter error\n";
        exit(-1);
    }

    if (cont != 'y') {
        std::cout << "Exit\n";
        exit(0);
    }
    else if (rank == 0) {
        std::cout << "Continuing\n";
    }
}

/*! The main functon for concatenation.
 *  @param[in] argc The number of arguments.
 *  @param[in] argv The array of cstrings.
 *  @details The options -i and -o must be specified, there are no defaults.
 *           -i \<pattern\> : input files (glob)
 *           -o \<pattern\> : output file prefix
 *           -m \<msg\> :  Message to write to the text header
 *           -p : Prompt the user to OK before concatenation
 *  @return Return zero on success, non-zero on failure.
 */
int main(int argc, char** argv)
{
    auto piol = ExSeis::make();

    std::string pattern;
    std::string outprefix;
    std::string msg = "Concatenated with ExSeisPIOL";
    bool prompt     = false;
    std::string opt = "i:o:m:p";  // TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
        switch (c) {
            case 'i':
                pattern = optarg;
                if (piol->get_rank() == 0) {
                    std::cout << "Pattern: " << pattern << "\n";
                }
                break;

            case 'o':
                outprefix = optarg;
                if (piol->get_rank() == 0) {
                    std::cout << "output prefix: " << outprefix << "\n";
                }
                break;

            case 'm':
                msg = optarg;
                break;

            case 'p':
                prompt = true;
                break;

            default:
                std::cerr << "One of the command line arguments is invalid\n";
                break;
        }
    }
    assert(!pattern.empty() && !outprefix.empty());

    Set set(piol, pattern);
    set.text(msg);
    if (prompt) {
        set.summary();
        do_prompt(piol.get());
    }
    set.output(outprefix);

    return 0;
}
