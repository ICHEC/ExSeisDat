#include <iostream>
#include <cassert>
#include "comm/mpi.hh"
#include "file/filesegy.hh"
#include "file/ops.hh"
#include "set/man.hh"
using namespace PIOL;
void getMinMaxCoords(Comms::MPI & comm, Set::Manager & man)
{
    if (!comm.getRank()) std::cout << "Read Coords\n";

    auto mcoord = man.readCoord(PIOL::File::Coord::Cmp);

    auto offset = mcoord.first;
    auto coord = mcoord.second;

    if (!comm.getRank()) std::cout << "Calc Min/Max\n";
    File::CoordData zerop = std::pair<real, real>(0.0, 0.0);

    real min = Ops::calcMin(comm, zerop, coord);
    real max = Ops::calcMax(comm, zerop, coord);

    if (!comm.getRank()) std::cout << "Find Min/Max\n";
    size_t findMin = Ops::findMin(comm, offset, zerop, coord);
    size_t findMax = Ops::findMax(comm, offset, zerop, coord);

    if (!comm.getRank()) std::cout << "Min " << min << " max = " << max << std::endl;
    if (!comm.getRank()) std::cout << "Trace Number, min " << findMin + 1 << " max " << findMax + 1 << std::endl;
}

int main(int argc, char ** argv)
{
    assert(argc > 1);
    std::string inFile(argv[1]);
    auto comm = std::make_shared<Comms::MPI>(MPI_COMM_WORLD);

    if (!comm->getRank()) std::cout << "In file " << inFile << std::endl;
    Set::Manager seg(comm, std::unique_ptr<File::Interface>(new File::SEGY(comm, inFile, PIOL::Block::Type::MPI)));

    if (!comm->getRank()) std::cout << "Header has been read. Now calc min max of  " << inFile << std::endl;

    getMinMaxCoords(*comm, seg);

    if (!comm->getRank()) std::cout << "Successful exit";
    return 0;
}

