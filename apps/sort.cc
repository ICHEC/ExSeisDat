#include "exseisdat/flow/Set.hh"
#include "exseisdat/piol/configuration/ExSeis.hh"

#include <assert.h>
#include <iostream>
#include <unistd.h>

using namespace exseis::piol;
using namespace exseis::flow;

int main(int argc, char** argv)
{
    auto piol       = ExSeis::make();
    std::string opt = "i:o:t:";  // TODO: uses a GNU extension

    std::string name1;
    std::string name2;

    Sort_type type = Sort_type::SrcRcv;
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
        switch (c) {
            case 'i':
                name1 = optarg;
                break;

            case 'o':
                name2 = optarg;
                break;

            case 't':
                type = static_cast<Sort_type>(std::stoul(optarg));
                break;

            default:
                std::cerr << "One of the command line arguments is invalid\n";
                break;
        }
    }

    assert(!name1.empty() && !name2.empty());

    Set set(piol, name1, name2);
    set.sort(type);
    piol->assert_ok();

    return 0;
}
