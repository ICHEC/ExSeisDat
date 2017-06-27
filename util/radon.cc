#define _POSIX_C_SOURCE 200809L //POSIX includes
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include "flow.hh"

using namespace PIOL;
int main(int argc, char ** argv)
{
    ExSeis piol;
    std::string opt = "i:o:v:";  //TODO: uses a GNU extension
    std::string radon = "", angle = "", velocity = "";
    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'i' :
                radon = optarg;
            break;
            case 'v' :
                velocity = optarg;
            break;
            case 'o' :
                angle = optarg;
            break;
            default :
                std::cerr << "One of the command line arguments is invalid\n";
            break;
        }

    assert(radon.size() && angle.size() && velocity.size());

    Set set(piol, radon, angle);
    set.toAngle(velocity, 60U, 20U);

    piol.isErr();
    if (!piol.getRank())
        std::cout << "Begin output\n";

    return 0;
}



