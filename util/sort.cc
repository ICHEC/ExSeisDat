#include <assert.h>
#include <iostream>
#include "set.hh"
#include "sglobal.hh"
using namespace PIOL;
int main(int argc, char ** argv)
{
    ExSeis piol;
    std::string opt = "i:o:t:d";  //TODO: uses a GNU extension
    std::string name1 = "";
    std::string name2 = "";
    auto type = File::SortType::SrcRcv;
    bool debug = false;
    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'i' :
                name1 = optarg;
            break;
            case 'o' :
                name2 = optarg;
            break;
            case 'd' :
                debug = true;
            break;
            case 't' :
                type = static_cast<File::SortType>(std::stoul(optarg));
            break;
            default :
                std::cerr << "One of the command line arguments is invalid\n";
            break;
        }

    assert(name1.size() && name2.size());

    if (!piol.getRank())
        std::cout << "Set\n";
    Set set(piol, name1, name2);
    if (!piol.getRank())
        std::cout << "Sort\n";
    set.sort(type);
    piol.isErr();
    if (!piol.getRank())
        std::cout << "Output\n";

    return 0;
}
