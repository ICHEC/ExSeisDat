// POSIX includes
#define _POSIX_C_SOURCE 200809L

#include "ExSeisDat/Flow/set.hh"
#include "ExSeisDat/PIOL/ExSeis.hh"

#include <assert.h>
#include <iostream>
#include <unistd.h>

using namespace PIOL;

int main(int argc, char** argv)
{
    auto piol         = ExSeis::New();
    std::string opt   = "i:o:t:";  // TODO: uses a GNU extension
    std::string name1 = "";
    std::string name2 = "";
    SortType type     = PIOL_SORTTYPE_SrcRcv;
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str()))
        switch (c) {
            case 'i':
                name1 = optarg;
                break;
            case 'o':
                name2 = optarg;
                break;
            case 't':
                type = static_cast<SortType>(std::stoul(optarg));
                break;
            default:
                std::cerr << "One of the command line arguments is invalid\n";
                break;
        }

    assert(name1.size() && name2.size());

    Set set(piol, name1, name2);
    set.sort(type);
    piol->isErr();

    return 0;
}
