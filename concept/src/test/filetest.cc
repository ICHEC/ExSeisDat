#include <vector>
#include <cassert>
#include <string>
#include <iostream>
#include <utility>
#include "global.hh"
#include "file/filesegy.hh"
#include "set/set.hh"
#include "parallel.hh"
#include "comm/mpi.hh"
using namespace PIOL;
using PIOL::File::Interface;

/*void getTestCoords(File::Interface & file)
{
    auto decomp = decompose(file.readNt());

    std::vector<Interface::CoordData> coord(decomp.second);

    file.readCoord(decomp.first, PIOL::File::Coord::Lin, coord);
}

void testPolymorphismGets(File::Interface & file)
{
    std::cout << "nt " << file.readNt() << std::endl;
    std::cout << "ns " << file.readNs() << std::endl;
    std::cout << "increment " << file.readInc() << std::endl;
    getTestCoords(file);
    getCoords(file);
//    TestData(file);
}*/

void copyTestPolymorphism(File::Interface & out, Set::Manager & in)
{
    std::cout << "Copy Data\n";
    auto header = in.readHeader();
    auto coords = in.readCoord();
    auto traces = in.readTraces();

    out.writeHeader(header);
    std::cout << "Write header done\n";

    out.writeCoord(coords);
    out.writeTraces(traces);

    //out.writeFile(header, coords, data);

//    TestData(file);

//    out = in;
}

int main(int argc, char ** argv)
{
    assert(argc > 2);
    std::string inFile(argv[1]);
    std::string outFile(argv[2]);

    //MPI Init
    auto comm = std::make_shared<Comms::MPI>(MPI_COMM_WORLD);

    std::cout << "In file " << inFile << std::endl;

    Set::Manager seg(comm, std::unique_ptr<File::Interface>(new File::SEGY(comm, inFile, PIOL::Block::Type::MPI)));

//    testPolymorphismGets(seg); 
    
    std::cout << "Out file " << outFile << std::endl;

    File::SEGY out(comm, outFile, PIOL::Block::Type::MPI);
    copyTestPolymorphism(out, seg);
    std::cout << "Terminate" << std::endl;
    return 0;
}

