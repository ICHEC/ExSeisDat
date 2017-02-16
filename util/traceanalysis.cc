 #include <unistd.h> //getopt
#include <iostream>
#include "cppfileapi.hh"
using namespace PIOL;
int main(int argc, char ** argv)
{
    ExSeis piol;
    std::string name = "";
    size_t tn = 0U;
    std::string opt = "i:t:";  //TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'i' :
                name = optarg;
            break;
            case 't' :
                tn = std::stoul(optarg);
            break;
            default :
                std::cerr<< "One of the command line arguments is invalid\n";
            break;
        }
    File::Direct file(piol, name, FileMode::Read);

    File::Param prm(1U);
    file->readParam(tn, 1U, &prm);
    if (!piol.getRank())
    {
        std::cout << "xSrc " << File::getPrm<geom_t>(0U, Meta::xSrc, &prm) << std::endl; 
        std::cout << "ySrc " << File::getPrm<geom_t>(0U, Meta::ySrc, &prm) << std::endl; 
        std::cout << "xRcv " << File::getPrm<geom_t>(0U, Meta::xRcv, &prm) << std::endl; 
        std::cout << "yRcv " << File::getPrm<geom_t>(0U, Meta::yRcv, &prm) << std::endl; 
        std::cout << "xCmp " << File::getPrm<geom_t>(0U, Meta::xCmp, &prm) << std::endl; 
        std::cout << "yCmp " << File::getPrm<geom_t>(0U, Meta::yCmp, &prm) << std::endl; 

        std::cout << "il " << File::getPrm<size_t>(0U, Meta::il, &prm) << std::endl; 
        std::cout << "xl " << File::getPrm<size_t>(0U, Meta::xl, &prm) << std::endl; 
    }
    return 0;
}

