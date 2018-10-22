#include "exseisdat/flow/Set.hh"
#include "exseisdat/piol/ExSeis.hh"

#include <assert.h>
#include <iostream>
#include <unistd.h>

using namespace exseis::piol;
using namespace exseis::flow;

int main(int argc, char** argv)
{
    auto piol       = ExSeis::make();
    std::string opt = "i:o:v:b:a:";  // TODO: uses a GNU extension

    std::string radon;
    std::string angle;
    std::string velocity;

    auto v_bin                  = 20LU;
    auto output_sample_interval = 60LU;
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
                v_bin = std::stoul(optarg);
                break;

            case 'a':
                output_sample_interval = std::stoul(optarg);
                break;

            default:
                std::cerr << "One of the command line arguments is invalid\n";
                break;
        }
    }

    assert(!radon.empty() && !angle.empty() && !velocity.empty());

    if (piol->get_rank() == 0) {
        std::cout << "Radon to Angle Transformation"
                  << "\n-\tInput radon file:\t" << radon
                  << "\n-\tVelocity model file:\t" << velocity
                  << "\n-\tOutput angle file:\t" << angle
                  << "\n-\tIncrement:\t\t" << output_sample_interval
                  << "\n-\tv_bin:\t\t\t" << v_bin << std::endl;
    }
    Set set(piol, radon, angle);
    piol->assert_ok();
    set.to_angle(velocity, v_bin, output_sample_interval);

    piol->assert_ok();
    if (piol->get_rank() == 0) {
        std::cout << "Begin output\n";
    }

    return 0;
}
