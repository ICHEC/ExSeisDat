#include "global.hh"
#include "ops/ops.hh"
#include "file/filesegy.hh"
#include <iostream>
using namespace PIOL;
using namespace File;
int calcMin(Piol piol, std::string name)
{
    Interface * file = new SEGY(piol, name);

    //Just a test, don't care about remainder
    size_t num = file->readNt() / piol->comm->getNumRank();
    size_t offset = num * piol->comm->getRank();
    std::vector<coord_t> buf;

    try
    {
        buf.resize(num);
    }
    catch (std::bad_alloc e)
    {
        std::cout << "Could not allocate vector" << std::endl;
        return -1;
    }

    file->readCoordPoint(Coord::Src, offset, num, buf.data());
    geom_t min = xmin(piol, offset, num, buf.data());

    if (!piol->comm->getRank())
        std::cout << "Minimum x for first " << num*piol->comm->getNumRank() << " traces is " << min << std::endl;
    return 0;
}

