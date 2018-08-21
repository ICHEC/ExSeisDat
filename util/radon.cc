// POSIX includes
#define _POSIX_C_SOURCE 200809L

#include "ExSeisDat/Flow/Set.hh"
#include "ExSeisDat/PIOL/ExSeis.hh"

#include <assert.h>
#include <iostream>
#include <unistd.h>

using namespace exseis::PIOL;
using namespace exseis::Flow;

int main(int argc, char** argv)
{
    auto piol       = ExSeis::New();
    std::string opt = "i:o:v:b:a:";  // TODO: uses a GNU extension

    std::string radon;
    std::string angle;
    std::string velocity;

    auto vBin = 20LU;
    auto oInc = 60LU;
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
        switch (c) {
            case 'i':
                radon = optarg;
                break;

            case 'v':
                velocity = optarg;
                break;

            case 'o':
                angle = optarg;
                break;

            case 'b':
                vBin = std::stoul(optarg);
                break;

            case 'a':
                oInc = std::stoul(optarg);
                break;

            default:
                std::cerr << "One of the command line arguments is invalid\n";
                break;
        }
    }

    assert(!radon.empty() && !angle.empty() && !velocity.empty());

    if (piol->getRank() == 0) {
        std::cout << "Radon to Angle Transformation"
                  << "\n-\tInput radon file:\t" << radon
                  << "\n-\tVelocity model file:\t" << velocity
                  << "\n-\tOutput angle file:\t" << angle
                  << "\n-\tIncrement:\t\t" << oInc << "\n-\tvBin:\t\t\t" << vBin
                  << std::endl;
    }
    Set set(piol, radon, angle);
    piol->isErr();
    set.toAngle(velocity, vBin, oInc);

    piol->isErr();
    if (piol->getRank() == 0) {
        std::cout << "Begin output\n";
    }

    return 0;
}
