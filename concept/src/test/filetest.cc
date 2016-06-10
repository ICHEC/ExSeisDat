#include <vector>
#include <cassert>
#include <string>
#include <iostream>
#include "file/SEGY.hh"

using namespace PIOL;
int main(int argc, char ** argv)
{
    //MPI Init
    int err = MPI_Init(&argc, &argv);
    PIOL::Block::MPI::printErr(err, NULL, "MPI_Init failure\n"); 

    assert(argc > 1);
    std::cout << "Open file " << argv[1] << std::endl;
    
    PIOL::File::SEGY::FileSEGY seg(MPI_COMM_WORLD, argv[1], PIOL::Block::Type::MPI);
        
    std::cout << "nt " << seg.getNt() << std::endl;
    std::cout << "ns " << seg.getNs() << std::endl;
    std::cout << "increment " << seg.getInc() << std::endl;

    //MPI Finalize
    err = MPI_Finalize();
    PIOL::Block::MPI::printErr(err, NULL, "MPI_Finalize failure\n");

    return 0;
}

