#include <vector>
#include <cassert>
#include <string>
#include <iostream>
#include <utility>
#include "file/SEGY.hh"

using namespace PIOL;
using PIOL::File::Interface;

void TestPolymorphism(File::Interface & file, size_t count)
{
    if (file.getNt() < count)
        return;

    std::cout << "nt " << file.getNt() << std::endl;
    std::cout << "ns " << file.getNs() << std::endl;
    std::cout << "increment " << file.getInc() << std::endl;

    Interface::CoordData Coords(count);

    file.getCoord(file.getNt()-count, PIOL::File::Coord::Lin, Coords);

    for (size_t i = 0; i < Coords.size(); i++)
    {
        std::cout << "Coord " << i << " " << Coords[i].first << " " << Coords[i].second << std::endl;
    }
}

int main(int argc, char ** argv)
{
    //MPI Init
    int err = MPI_Init(&argc, &argv);
    Block::MPI::printErr(err, NULL, "MPI_Init failure\n"); 

    assert(argc > 1);
    std::cout << "Open file " << argv[1] << std::endl;
    
    File::SEGY::Interface seg(MPI_COMM_WORLD, argv[1], PIOL::Block::Type::MPI);

    TestPolymorphism(seg, 10); 
    
    //MPI Finalize
    err = MPI_Finalize();
    Block::MPI::printErr(err, NULL, "MPI_Finalize failure\n");

    return 0;
}

