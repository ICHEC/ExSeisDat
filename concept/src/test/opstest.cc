#include <iostream>
#include <cassert>
#include "comm/mpi.hh"
#include "file/filesegy.hh"
#include "ops/ops.hh"
#include "set/man.hh"
using namespace PIOL;

template <typename T>
void printMinMax(Comms::MPI & comm, std::pair<size_t, std::vector<File::Pair<T>> > & mdata)
{
    auto offset = mdata.first;
    auto data = mdata.second;
    auto zerop = std::pair<T, T>(T(0), T(0));

    if (!comm.getRank()) std::cout << "Calc Min/Max\n";

    T min = Ops::calcMin(comm, zerop, data);
    T max = Ops::calcMax(comm, zerop, data);

    if (!comm.getRank()) std::cout << "Find Min/Max\n";

    size_t findMin = Ops::findMin(comm, offset, zerop, data);
    size_t findMax = Ops::findMax(comm, offset, zerop, data);

    if (!comm.getRank()) std::cout << "Min " << min << " max = " << max << std::endl;
    if (!comm.getRank()) std::cout << "Trace Number, min " << findMin + 1 << " max " << findMax + 1 << std::endl;
}

void getMinMaxCoords(Comms::MPI & comm, Set::Manager & man)
{
    if (!comm.getRank()) std::cout << "Read Cmp\n";
    auto mcoord = man.readCoord(PIOL::File::Coord::Cmp);
    if (!comm.getRank()) std::cout << "Read Lin\n";
    auto mgrid = man.readGrid(PIOL::File::Grid::Lin);

    if (!comm.getRank()) std::cout << "Max Lin\n";
    printMinMax(comm, mgrid);

    if (!comm.getRank()) std::cout << "Max Cmp\n";
    printMinMax(comm, mcoord);

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

