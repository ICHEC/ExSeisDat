/* Cathal O Broin - cathal.obroin4 at mail.dcu.ie - 2015-2016
   This work is not developed in affiliation with any organisation.

   This file is part of AILM.

   AILM is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   AILM is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with AILM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <string>
#include <array>
#include <cmath>
#include "mpi-io.hh"
#include "parallel.hh"

namespace iotest
{
template <class T>
void read(std::pair<size_t, size_t> Div, std::string Name, std::vector<T> & Data, size_t Global)
{
    io::MPIRead(Div, Name, Data);
}
template <class T>
void write(std::pair<size_t, size_t> Div, std::string Name, std::vector<T> & Data, size_t Global)
{
    io::MPIWrite(Div, Name, Data);
}


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
void WriteReadTest(int Rank, int NumRank, std::string Name, size_t Global, std::array<real, GMax> & GTime)
{
    real t1, t2 = MPI_Wtime(), IOWrite, IORead, Calc;

    auto Div = parallel::distrib(Rank, NumRank, Global);
    auto Sz = Div.second - Div.first;
    {
        std::vector<T> Data(Sz);
//        #pragma omp parallel for 
        for (size_t i = 0U; i < Sz; i++)
            Data[i] = real(i + Div.first); //overflow is ok

        t1 = MPI_Wtime();
        Calc = t1 - t2;
        io::MPIWrite(Div, Name, Data);

        t2 = MPI_Wtime();
        IOWrite = t2 - t1;

    }//Scoped so that Data is destroyed.

    int Fail = 0;

    int PrintFail = 0;
    {
        std::vector<T> Data(Sz);
        io::MPIRead(Div, Name, Data, MPI_MODE_DELETE_ON_CLOSE);

        t1 = MPI_Wtime();
        IORead = t1 - t2;

        for (size_t i = 0U; i < Sz; i++)
            Fail += int(Data[i] != real(i + Div.first));
        t2 = MPI_Wtime();
        Calc = t2 - t1;
    }
    int TotalFail = 0;
    if (MPI_Reduce(&Fail, &TotalFail, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD) != MPI_SUCCESS)
    {
        std::cerr << "MPI Reduction has failed\n";
        MPI_Finalize(); 
        exit(-1);
    }

    std::array<real, GMax> Time;
    std::array<real, GMax> GetTime;

    Time.at(GIOWrite) = IOWrite;
    Time.at(GIORead) = IORead;
    Time.at(GCalc) = Calc;

    if (MPI_Reduce(Time.data(), GetTime.data(), GetTime.size(), MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD) != MPI_SUCCESS)
    {
        std::cerr << "MPI Reduction Double has failed\n";
        MPI_Finalize(); 
        exit(-1);
    }
    else if (!Rank)
    {
        t1 = MPI_Wtime();
        GetTime.at(GReduce) = (t1-t2) * NumRank;

        for (int i = 0; i < GTime.size(); i++)
            GTime.at(i) += GetTime.at(i);
        std::cout << "Did it work? " << (TotalFail ? "no " : "yes ") << TotalFail << std::endl;
    }
}

void PrintStats(int Rank, int NumRank, std::array<real, GMax> & GTime)
{
    if (!Rank)
    {
        std::cout << "Total IO Write time:\n" << GTime.at(iotest::GIOWrite) / NumRank << std::endl;
        std::cout << "Total IO Read time:\n" << GTime.at(iotest::GIORead) / NumRank << std::endl;
        std::cout << "Total Reduce (Rank 0 only) time:\n" << GTime.at(iotest::GReduce) / NumRank << std::endl;
        std::cout << "Total Calc Time:\n" << GTime.at(iotest::GCalc) / NumRank << std::endl;
    }
}
}
#include <limits>
int main(int argc, char ** argv)
{
    std::array<real, iotest::GMax> GTime;
    int Loop;
    if (argc < 3)
    {
        std::cerr << "Insufficient number of arguments\n";
        return EXIT_FAILURE;
    }

    int NumRank, Rank, Total;
    if (MPI_Init(&argc, &argv) != MPI_SUCCESS)
    {
        std::cerr << "MPI Initialize failure\n";
        return EXIT_FAILURE;
    }
    if (MPI_Comm_rank(MPI_COMM_WORLD, &Rank) != MPI_SUCCESS)
    {
        std::cerr << "MPI Rank acquire failure\n";
        return EXIT_FAILURE;
    }
    if (MPI_Comm_size(MPI_COMM_WORLD, &NumRank) != MPI_SUCCESS)
    {
        std::cerr << "MPI Size acquire failure\n";
        return EXIT_FAILURE;
    }

    std::string Name(argv[1]);
    std::string SSz(argv[2]);
    size_t Sz;
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

    if (Rank == 0)
    {
        std::cout << "Max File: " <<  pow(2, 8*sizeof(size_t) - 30) << " " << std::numeric_limits<size_t>::max() / (1024*1024*1024) << " GiB\n";
        std::cout << "Max Int File: " <<  pow(2, 8*sizeof(int) - 30) << " " << std::numeric_limits<int>::max() / (1024*1024*1024) << " GiB\n";
        std::cout << "Max Offset File: " <<  pow(2, 8*sizeof(MPI_Offset) - 31) << " GiB\n";
        std::cout << "Sz " << Sz << " doubles, " << Loop << " loops.\n";
        std::cout << "Pre-main loop\n";
    }
    for (int i = 0; i < Loop; i++)
    {
        if (Rank == 0)
            std::cout << "Loop " << i+1 << " of " << Loop << std::endl;
        iotest::WriteReadTest<real>(Rank, NumRank, Name, Sz, GTime);
    }
    if (Rank == 0)
    {
        iotest::PrintStats(Rank, NumRank, GTime);
        std::cout << "Terminating\n";
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (MPI_Finalize() != MPI_SUCCESS)
    {
        std::cerr << "MPI Finalize failure\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
