/* ExSeisDat block test
1. Initial write - cathal.obroin@ichec.ie 2016
2. modify to use block layer - cathal - June 2016
*/

/*
    Assumptions:
    Each MPI Process can allocate their chunk of memory
    There will be an equal number MPI-IO calls per process

*/

#include <iostream>
#include <string>
#include <array>
#include <cmath>
#include <functional>
#include <omp.h>
#include <unistd.h>
#define TEST_PRIVATE
#include "block/blockmpiio.hh"
#include "parallel.hh"
#include "comm/mpi.hh"
using namespace PIOL;

namespace iotest
{
enum : size_t
{
    GIOWrite = 0,
    GIORead,
    GCalc,
    GReduce,
    GMax
};

void Print(size_t Rank, std::string Msg)
{
    if (!Rank)
        std::cout << Msg << std::endl;
}

template <class T>
void WriteTest(std::shared_ptr<Comms::MPI> comm, std::string Name, size_t Global, std::function<T(size_t)> fn)
{
    auto Div = parallel::distrib<size_t>(comm->getRank(), comm->getNumRank(), Global);
    auto Sz = Div.second - Div.first;
    std::cout << "Size = " << Sz << std::endl;

    T * Data = new T[Sz];
    if (!Data)
        throw(-1);
//    std::vector<T> Data(Sz);

    Print(comm->getRank(), "Data calc start\n");
    #pragma omp simd
    for (size_t i = 0U; i < Sz; i++)
        Data[i] = fn(i + Div.first); //overflow of type T is fine

    MPI_Barrier(MPI_COMM_WORLD);
    if (comm->getRank() == 0)
        std::cout << "Data calc done\n";

//I/O
    std::cout << "Open File " << Name << std::endl;
    Block::MPIIO out(comm, Name);
    out.setFileSz(Global*sizeof(T));
    out.writeData(Div.first, Data, Sz);

    delete[] Data;
}

template <class T>
void ReadTest(std::shared_ptr<Comms::MPI> comm, std::string Name, size_t Global, std::function<T(size_t)> fn)
{
    auto Div = parallel::distrib<size_t>(comm->getRank(), comm->getNumRank(), Global);
    auto Sz = size_t(Div.second - Div.first);
    T * Data = new T[Sz];
    if (!Data)
        throw(-1);

    {
        Block::MPIIO in(comm, Name, MPI_MODE_UNIQUE_OPEN | MPI_MODE_RDONLY);
        //Block::MPI::Interface<MPI_Status> in(Name);
        in.readData(Div.first, Data, Sz);
    }
    unsigned long int Fail = 0;
    unsigned long int TotalFail = 0;

//    #pragma omp simd reduction(+:Fail)
    for (size_t i = 0U; i < Sz; i++)
        if (Data[i] != fn(i + Div.first))
            Fail++;

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(&Fail, &TotalFail, 1, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (!comm->getRank())
        std::cout << "Test Success: " << TotalFail << std::endl;
    
    delete[] Data;
}
}
#include <limits>

template <class T>
T lightFunc(size_t i)
{
    return T(i % 1024*1024);
}
char charFunc(size_t i)
{
    return char(48U + i % 42U);
}

int heavyFunc(size_t i)
{
    int ret = i%97;
    return int(pow(ret, 3) / pow(ret+i%33, 2)) + ret*2;
}

struct IOMode
{
    unsigned int IORead : 1;
    unsigned int IOWrite : 1;
};

//TODO: Add proper option control
int main(int argc, char ** argv)
{
    int ExtraInfo = 0, Err;
    IOMode Mode = {0, 0}; //Default on

    auto comm = std::make_shared<Comms::MPI>(MPI_COMM_WORLD);

    if (argc < 3)
    {
        std::cerr << "Insufficient number of arguments\n";
        return EXIT_FAILURE;
    }

    std::string Name(argv[1]);
    size_t Sz;
    {
        std::string SSz(argv[2]);
        try
        {
            Sz = std::stoul(SSz); //Quick and dirty, not concerned about errors
        }
        catch (std::invalid_argument e)
        {
            std::cerr << "Command-line argument is invalid for the type conversion used\n";
            return EXIT_FAILURE;
        }
        catch (std::out_of_range e)
        {
            std::cerr << "Command-line argument is out of range. \n";
            return EXIT_FAILURE;
        }
    }
    if (argc >= 3) 
        try
        {
            std::string SSz(argv[3]);
            Mode.IOWrite = std::stoi(SSz);
            Mode.IORead = !Mode.IOWrite;
        }
        catch (...)
        {
            Mode.IOWrite = 1;
        }

    if (comm->getRank() == 0 && ExtraInfo)
    {
        std::cout << "Max File: " <<  pow(2, 8*sizeof(size_t) - 30) << " " << std::numeric_limits<size_t>::max() / (pow(2, 60)) << " EiB\n";
        std::cout << "Max Int File: " <<  pow(2, 8*sizeof(int) - 30) << " " << std::numeric_limits<int>::max() / (pow(2, 60)) << " EiB\n";
        std::cout << "Max Offset File: " <<  std::numeric_limits<MPI_Offset>::max() / (pow(2, 60))  << " EiB\n";
        std::cout << "Max uint: " <<  std::numeric_limits<unsigned int>::max() << std::endl;
        std::cout << "Sz " << Sz << " elements\n";
        std::cout << "Pre-main loop\n";
        std::cout << "Rank: "  << comm->getNumRank() << std::endl;
    }

    if (ExtraInfo)
        for (size_t i = 0; i < comm->getNumRank(); i++)
        {
            if (i == comm->getRank())
            {
                char gName[1024];
                gethostname(gName, 1024);
                std::cout << comm->getRank() << " " << gName << std::endl;
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }

    if (Mode.IOWrite)
    {
        if (comm->getRank() == 0)
            std::cout << "Writing " << Sz << ".\n"; 
        iotest::WriteTest<float>(comm, Name, Sz, lightFunc<float>);
        //iotest::WriteTest<char>(Rank, NumRank, Name, Sz, charFunc);
    }
    if (Mode.IORead)
    {
        if (comm->getRank() == 0)
            std::cout << "Reading " << Sz << " .\n";
       // iotest::ReadTest<char>(Rank, NumRank, Name, Sz, charFunc);
        iotest::ReadTest<float>(comm, Name, Sz, lightFunc<float>);
    }

    if (comm->getRank() == 0 && ExtraInfo)
        std::cout << "Terminating\n";

    return EXIT_SUCCESS;
}
