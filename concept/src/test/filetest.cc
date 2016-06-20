#include <vector>
#include <cassert>
#include <string>
#include <iostream>
#include <utility>
#include "global.hh"
#include "file/SEGY.hh"
#include "parallel.hh"
#include "comm/mpi.hh"
using namespace PIOL;
using PIOL::File::Interface;

std::pair<size_t, size_t> getMPIDecomp(size_t num)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int numRank;
    MPI_Comm_size(MPI_COMM_WORLD, &numRank);

    auto decomp = parallel::distrib<size_t>(size_t(rank), size_t(numRank), num);
    //TODO: Check decomp.second > decomp.first

    return std::make_pair(decomp.first, decomp.second-decomp.first);
}

void getTestCoords(File::Interface & file)
{
    auto decomp = getMPIDecomp(file.readNt());

    std::vector<Interface::CoordData> coord(decomp.second);

    file.readCoord(decomp.first, PIOL::File::Coord::Lin, coord);

//TODO: Perform test here

/*    for (auto i = 0; i < numRank; i++)
    {
        if (i == rank)
        {
            std::cout << "Rank" << i << " " << coord.size();
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
*/
}

std::vector<Interface::CoordArray> getCoords(File::Interface & file)
{
    auto decomp = getMPIDecomp(file.readNt());

    std::vector<Interface::CoordArray> allCoords(decomp.second);
    file.readCoord(decomp.first, allCoords);
    return allCoords;

//TODO: Perform test here
/*    for (size_t i = 0; i < allCoords.size(); i++)
    {
        for (size_t j = 0; j < allCoords[i].size(); j++)
        {
            std::cout << i << " " << j << " " << allCoords[i][j].first << " " << allCoords[i][j].second << std::endl;
        }
    }*/
}

//TODO: This is broken. I'm parking it for now.
float makeIEEE(uint32_t sign, uint32_t frac, uint32_t exp)
{
    union { float d; uint32_t i; } src;

    if (frac < 0x800000)
    {
        frac >> 22;

        exp++;
        frac <<= 1;
    }
    exp++;

    src.i = sign | ((exp + 0x0000007F)<< 23) | (frac & 0x003FFFFF) ;

    for (int i = 31; i >= 0; i--)
    {
        std::cout << ((src.i >> i) & 0x1);
    }
    std::cout << " ";

    std::cout << "makeIEEE" << src.d << std::endl;
    return src.d;
}

template <typename T>
T convIBM(T data)
{
    union { float d; uint32_t i; } src;
    src.d = data;

    uint32_t uidata = src.i;

    std::cout << "\nIBM\t";
    std::cout << " ";
    uint32_t sign = uidata & 0x80000000;
    uint32_t frac = uidata & 0x00FFFFFF;
    uint32_t exp = uidata & 0x7F000000 >> 23 - 0x4;
    exp <<= 4; //raise to power of 2^4

//    std::cout << "IBM " << data << " " << uidata<< " " << (sign ? -1.0 : 1.0) << " " << (2 << exp) << " " << frac << std::endl;

    return makeIEEE(sign, exp, frac);
}

/*template <typename T>
T makeHostEndian(T d)
{
    assert(sizeof(T) == 4);
    union { float d; uint32_t i; } src;
    src.d = d;
    return (0x000F & src.i) << 24 |
           (0x00F0 & src.i) << 8 |
           (0x0F00 & src.i) >> 8 |
           (0xF000 & src.i) >> 24; 
}*/
extern void ibm2ieee( void *to, const void *from, long long);
//TODO: Check about data copy cost
std::vector<real> getAllTraces(File::Interface & file)
{
    auto decomp = getMPIDecomp(file.readNt());
    std::vector<real> data(decomp.second * file.readNs());
    file.readTraces(0, data);
//TODO: omp target
    for (size_t i = 0; i < file.readNs(); i++)
    {
        for (size_t j = 0; j < data.size() / file.readNs(); j++)
        {
            ibm2ieee(&data[j*file.readNs() + i], &data[j*file.readNs() + i], 1);
        }
    }
    std::cout << "getAllTraces First " << data[0] << std::endl;
    return data;
    //for (size_t i = 0; i < file.getNs(); i++)
/*    size_t i = 128;
    {
        std::cout << i << "\t";
        for (size_t j = 0; j < data.size() / file.getNs(); j++)
        {
            //std::cout << makeHostEndian(data[j*file.getNs() + i]) << "\t";
            real test2 = convIBM(data[j*file.getNs() + i]);
            union { real d; unsigned int i; } d;
            d.d = test2;
            std::cout << "IBMC ";
            for (int k = 31; k >= 0; k--)
            {
                std::cout << ((d.i >> k) & 0x1);
            }
            std::cout <<"\n" << test2 << "\t";

            real test;
            ibm2ieee(&test, &data[j*file.getNs() + i], 1);

            d.d = test;
            std::cout << "WOrking ";
            for (int k = 31; k >= 0; k--)
            {
                std::cout << ((d.i >> k) & 0x1);
            }


            std::cout << test << "\t";
        }
        std::cout << std::endl;
    }*/
}

void testPolymorphismGets(File::Interface & file)
{
    std::cout << "nt " << file.readNt() << std::endl;
    std::cout << "ns " << file.readNs() << std::endl;
    std::cout << "increment " << file.readInc() << std::endl;
    getTestCoords(file);
    getCoords(file);
//    TestData(file);
}

void copyTestPolymorphism(File::Interface & out, File::Interface & in)
{
    std::cout << "Copy Data\n";
    auto header = in.readHeader();
    auto coords = getCoords(in);
    auto traces = getAllTraces(in);

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

    File::SEGY::Interface seg(comm, inFile, PIOL::Block::Type::MPI);

    testPolymorphismGets(seg); 
    
    std::cout << "Out file " << outFile << std::endl;

    File::SEGY::Interface out(comm, outFile, PIOL::Block::Type::MPI);
    copyTestPolymorphism(out, seg);
    std::cout << "Terminate" << std::endl;
    return 0;
}

