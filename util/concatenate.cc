////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date Q4 2016
/// @brief
/// @details This function takes one or more files as input and produces a new
///          file or files which contain all traces with identical ns and
///          increment.
////////////////////////////////////////////////////////////////////////////////

#include "flow.hh"
#include "sglobal.hh"
#include "share/api.hh"

#include <assert.h>
#include <iostream>

using namespace PIOL;

/*! Prompt the user asking them if they want to continue with concatenation.
 *  Multi-process safe.
 *  @param[in] piol The piol object.
 */
void doPrompt(ExSeisPIOL* piol)
{
    char cont   = '0';
    size_t rank = piol->comm->getRank();
    if (!rank) {
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
    else if (!rank)
        std::cout << "Continuing\n";
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
    auto piol = ExSeis::New();

    std::string pattern   = "";
    std::string outprefix = "";
    std::string msg       = "Concatenated with ExSeisPIOL";
    bool prompt           = false;
    std::string opt       = "i:o:m:p";  // TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str()))
        switch (c) {
            case 'i':
                pattern = optarg;
                if (!piol->getRank())
                    std::cout << "Pattern: " << pattern << "\n";
                break;
            case 'o':
                outprefix = optarg;
                if (!piol->getRank())
                    std::cout << "output prefix: " << outprefix << "\n";
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
    assert(pattern != "" && outprefix != "");

    Set set(piol, pattern);
    set.text(msg);
    if (prompt) {
        set.summary();
        doPrompt(piol.get());
    }
    set.output(outprefix);

    return 0;
}
