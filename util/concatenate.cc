#include <assert.h>
#include "set.hh"
#include "sglobal.hh"
#include <iostream>
using namespace PIOL;

void doPrompt(ExSeisPIOL * piol)
{
    char cont = '0';
    if (!piol->comm->getRank())
    {
        std::cout << "Continue concatenation? (y/n)\n";
        std::cin >> cont;
    }
    int err = MPI_Bcast(&cont, 1U, MPI_CHAR, 0, MPI_COMM_WORLD);
    if (err != MPI_SUCCESS)
    {
        std::cerr << "Scatter error\n";
        exit(-1);
    }

    if (cont != 'y')
    {
        std::cout << "Exit\n";
        exit(0);
    }
    else if (!piol->comm->getRank())
        std::cout << "Continuing\n";
}

int main(int argc, char ** argv)
{
    ExSeis piol;
    if (argc < 2)
    {
        std::cout << "Too few arguments\n";
        return -1;
    }

    std::string pattern = "";
    std::string outprefix = "";
    std::string msg = "Concatenated with ExSeisPIOL";
    bool prompt = false;
    std::string opt = "i:o:m:p";  //TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'i' :
                pattern = optarg;
                if (!piol.getRank())
                    std::cout << "Pattern: " << pattern << "\n";
            break;
            case 'o' :
                outprefix = optarg;
                if (!piol.getRank())
                    std::cout << "output prefix: " << outprefix << "\n";
            break;
            case 'm' :
                msg = optarg;
            break;
            case 'p' :
                prompt = true;
            break;
            default :
                std::cerr<< "One of the command line arguments is invalid\n";
            break;
        }

    if (prompt)
    {
        Set set(piol, pattern, std::make_shared<File::Rule>(true, true));
        set.summary();
        set.text(msg);
        if (prompt)
            doPrompt(piol);
        set.output(outprefix);
    }
    else
        Set(piol, pattern, outprefix, std::make_shared<File::Rule>(true, true));

    return 0;
}
