#include <vector>
#include <cassert>
#include <string>
#include <iostream>
#include <utility>
#include "file/SEGY.hh"
#include "parallel.hh"

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

    return std::make_pair(decomp.second-decomp.first, decomp.first);
}

void getTestCoords(File::Interface & file)
{
    auto decomp = getMPIDecomp(file.getNt());

    std::vector<Interface::CoordData> coord(decomp.second);

    file.getCoord(decomp.first, PIOL::File::Coord::Lin, coord);

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

std::vector<Interface::CoordArray> getAllCoords(File::Interface & file)
{
    auto decomp = getMPIDecomp(file.getNt());

    std::vector<Interface::CoordArray> allCoords(decomp.second);

    file.getAllCoords(decomp.first, allCoords);
    return allCords;

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
void getAllData(File::Interface & file)
{
    std::vector<real> data(decomp.second * file.getNs());
    file.getTraces(0, data);
//TODO: omp target
    for (size_t i = 0; i < file.getNs(); i++)
    {
        for (size_t j = 0; j < data.size() / file.getNs(); j++)
        {
            ibm2ieee(&test, &data[j*file.getNs() + i], 1);
        }
    }
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

void getTestPolymorphism(File::Interface & file)
{
    std::cout << "nt " << file.getNt() << std::endl;
    std::cout << "ns " << file.getNs() << std::endl;
    std::cout << "increment " << file.getInc() << std::endl;
    getTestCoords(file);
    getTestAllCoords(file);
//    TestData(file);
}

void copyTestPolymorphism(File::Interface & out, File::Interface & in)
{
    auto coords = getAllCoords(in);
    auto data = getAllData(in);

    setAllDO(out, coords, data);
//    TestData(file);

//    out = in;
}

int main(int argc, char ** argv)
{
    //MPI Init
    int err = MPI_Init(&argc, &argv);
    Block::MPI::printErr(err, NULL, "MPI_Init failure\n"); 

    assert(argc > 1);
    std::cout << "Open file " << argv[1] << std::endl;
    
    File::SEGY::Interface seg(MPI_COMM_WORLD, argv[1], PIOL::Block::Type::MPI);

    TestPolymorphism(seg); 
    
    File::SEGY::Interface out(MPI_COMM_WORLD, argv[2], PIOL::Block::Type::MPI);
    copyTest(out, seg);
    
    //MPI Finalize
    err = MPI_Finalize();
    Block::MPI::printErr(err, NULL, "MPI_Finalize failure\n");

    return 0;
}

