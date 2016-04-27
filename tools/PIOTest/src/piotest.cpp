/* ExSeisDat IO test tool
Initial write - cathal.obroin@ichec.ie 2016 */

#include <iostream>
#include <string>
#include <array>
#include <cmath>
#include <omp.h>
#include <unistd.h>
#include "mpi-io.hh"
#include "parallel.hh"

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

void Print(int Rank, std::string & Msg)
{
    if (!Rank)
        std::cout << Msg << std::endl;
}
template <class T>
void WriteReadTest(int Rank, int NumRank, std::string Name, size_t Global)
{
    auto Div = parallel::distrib<MPI_Offset>(Rank, NumRank, MPI_Offset(Global));
    auto Sz = size_t(Div.second - Div.first);

    {
        std::vector<T> Data(Sz);
        T f = T(Div.first);

        #pragma omp simd
        for (size_t i = 0U; i < Sz; i++)
            Data[i] = T(i) + f; //overflow of type T is fine

        MPI_File File = io::Open(MPI_COMM_WORLD, Name, MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_WRONLY);
        io::View<T>(File);

        int Err = MPI_File_set_size(File, Global*sizeof(double));
        io::MPIErr(Err, NULL, "Error resizing file\n");

        auto MPIWrite = [] (MPI_File f, MPI_Offset o, void * d, int s, MPI_Datatype da, MPI_Status * st) { return MPI_File_write_at(f, o, d, s, da, st); };
        io::MPIIO(MPIWrite, File, Div.first, Data);
        MPI_File_close(&File);


    }//Scoped so that Data is destroyed.

    int Fail = 0;
    {
        std::vector<T> Data(Sz);
        MPI_File File = io::Open(MPI_COMM_WORLD, Name, MPI_MODE_UNIQUE_OPEN | MPI_MODE_RDONLY); // | MPI_MODE_DELETE_ON_CLOSE);
        io::View<T>(File);
        io::MPIIO(MPI_File_read_at, File, Div.first, Data);
        MPI_File_close(&File);

        auto f = T(Div.first);
        int FailTest = 0;
        #pragma omp simd reduction(+:Fail)
        for (size_t i = 0U; i < Sz; i++)
            Fail += int(Data[i] != T(i) + f);
    }
    std::cout << "Rank " << Rank << (Fail ? " FAIL: " : " PASS: ") << Fail << std::endl;
}
}
#include <limits>
int main(int argc, char ** argv)
{
    int ExtraInfo = 0, Err;
    int Loop;
    if (argc < 3)
    {
        std::cerr << "Insufficient number of arguments\n";
        return EXIT_FAILURE;
    }

    int NumRank, Rank, Total;
    Err = MPI_Init(&argc, &argv);
    io::MPIErr(Err, NULL, "MPI_Init failure\n"); 

    Err = MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
    io::MPIErr(Err, NULL, "MPI_Comm_rank failure\n"); 

    MPI_Comm_size(MPI_COMM_WORLD, &NumRank);
    io::MPIErr(Err, NULL, "MPI_Comm_size failure\n"); 

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
            Loop = std::stoi(SSz);
        }
        catch (...)
        {
            Loop = 1;
        }

    if (Rank == 0 && ExtraInfo)
    {
        std::cout << "Max File: " <<  pow(2, 8*sizeof(size_t) - 30) << " " << std::numeric_limits<size_t>::max() / (pow(2, 60)) << " EiB\n";
        std::cout << "Max Int File: " <<  pow(2, 8*sizeof(int) - 30) << " " << std::numeric_limits<int>::max() / (pow(2, 60)) << " EiB\n";
        std::cout << "Max Offset File: " <<  std::numeric_limits<MPI_Offset>::max() / (pow(2, 60))  << " EiB\n";
        std::cout << "Sz " << Sz << " doubles, " << Loop << " loops.\n";
        std::cout << "Pre-main loop\n";
    }

    if (ExtraInfo)
        for (int i = 0; i < NumRank; i++)
        {
            if (i == Rank)
            {
                char Name[1024];
                gethostname(Name, 1024);
                std::cout << Rank << " " << Name << std::endl;
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }

    for (int i = 0; i < Loop; i++)
    {
        if (Rank == 0)
            std::cout << "Loop " << i+1 << " of " << Loop << std::endl;
        iotest::WriteReadTest<real>(Rank, NumRank, Name, Sz);
        std::string Name2 = Name + "2";
        iotest::WriteReadTest<char>(Rank, NumRank, Name2, Sz);
    }

    if (Rank == 0 && ExtraInfo)
        std::cout << "Terminating\n";
    MPI_Barrier(MPI_COMM_WORLD);

    Err = MPI_Finalize();
    io::MPIErr(Err, NULL, "MPI_Finalize failure\n"); 

    return EXIT_SUCCESS;
}
