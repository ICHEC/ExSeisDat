#define _POSIX_C_SOURCE 200809L //POSIX includes
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include "flow.hh"
using namespace PIOL;
int main(int argc, char ** argv)
{
    ExSeis piol;
    std::string opt = "i:o:t:";  //TODO: uses a GNU extension
    std::string name1 = "";
    std::string name2 = "";
    auto type = SortType::SrcRcv;
    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'i' :
                name1 = optarg;
            break;
            case 'o' :
                name2 = optarg;
            break;
            case 't' :
                type = static_cast<SortType>(std::stoul(optarg));
            break;
            default :
                std::cerr << "One of the command line arguments is invalid\n";
            break;
        }

    assert(name1.size() && name2.size());

    Set set(piol, name1, name2);
    set.sort(type);
    piol.isErr();
    if (!piol.getRank())
        std::cout << "Sort complete. Begin output\n";

    return 0;
}
