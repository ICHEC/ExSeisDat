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
#include <memory>
#include <iostream>
#include <mpi.h>
#include <limits>
#include "mpi-io.hh"
namespace io
{

typedef int (* MPI_Fp)(MPI_File, void *, int, MPI_Datatype, MPI_Status *);
constexpr size_t MPILimits(const size_t Chunk)
{
    //If you aren't (4096 - Chunk)/Chunk from the limit, intel mpi breaks.
    //Probably something to do with pages.
    return size_t(std::numeric_limits<int>::max() - (4096 - Chunk)) / Chunk;
}

void MPIIO(MPI_Fp Fn, MPI_File & File, size_t Sz, real * D)
{
    MPI_Status Stat;
    int Err;
    const size_t MPIMax = MPILimits(sizeof(double));
    if (Sz <= MPIMax)
        Err = Fn(File, D, Sz, MPI_DOUBLE, &Stat);
    else
    {
        size_t q = Sz / MPIMax;
        size_t r = Sz % MPIMax;
        int Rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &Rank);

        for (size_t i = 0; i < q; i++)
        {
            std::cout << Rank << " RW " << Sz << " " << i*MPIMax << " " << q << " " << r <<  std::endl;
            Err = Fn(File, &D[i*MPIMax], MPIMax, MPI_DOUBLE, &Stat);
            if (Err == MPI_ERR_IN_STATUS)
                break;
        }
        if (Err != MPI_ERR_IN_STATUS)
            Err = Fn(File, &D[Sz-r], r, MPI_DOUBLE, &Stat);
    }
    if (Err == MPI_ERR_IN_STATUS)
    {
        std::cout << "MPI_File_write failure: " << Stat.MPI_ERROR << std::endl;
        exit(EXIT_FAILURE);
    }
}

typedef int (* MPI_FpAt)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, MPI_Status *);
void MPIIO(MPI_FpAt Fn, MPI_File & File, MPI_Offset Offset, size_t Sz, real * D)
{
    MPI_Status Stat;
    int Err;
    const size_t MPIMax = MPILimits(sizeof(double));
    MPI_Offset q = Sz / MPIMax;
    MPI_Offset r = Sz % MPIMax;

    for (MPI_Offset i = 0; i < q; i++)
    {
        Err = Fn(File, Offset + i*MPIMax, &D[i*MPIMax], MPIMax, MPI_DOUBLE, &Stat);
        if (Err == MPI_ERR_IN_STATUS)
            break;
    }

    if (Err != MPI_ERR_IN_STATUS)
        Err = Fn(File, Offset + Sz-r, &D[Sz-r], r, MPI_DOUBLE, &Stat);

    if (Err == MPI_ERR_IN_STATUS)
    {
        std::cout << "MPI_File_write failure: " << Stat.MPI_ERROR << std::endl;
        exit(EXIT_FAILURE);
    }
}

void MPIRead(MPI_Comm Comm, std::string Name, MPI_Offset Offset, size_t Sz, real * D, int Mode = 0)
{
    Mode |= MPI_MODE_UNIQUE_OPEN | MPI_MODE_RDONLY;
    MPI_Info Info = MPI_INFO_NULL;
    MPI_File File = MPI_FILE_NULL;
    int Err = MPI_File_open(Comm, Name.c_str(), Mode, Info, &File);
    if (Err != MPI_SUCCESS)
    {
        std::cout << "MPI_File failure: " << Err << std::endl;
        exit(EXIT_FAILURE);
    }
    //Err = MPI_File_set_view(File, Offset, MPI_DOUBLE, MPI_DOUBLE, "native", Info);
    Err = MPI_File_set_view(File, 0, MPI_DOUBLE, MPI_DOUBLE, "native", Info);
    if (Err != MPI_SUCCESS)
    {
        std::cout << "set MPI view failure: " << Err << std::endl;
        exit(EXIT_FAILURE);
    }

    MPIIO(MPI_File_read_at, File, Offset, Sz, D);
//    MPIIO(MPI_File_read, File, Sz, D);
    MPI_File_close(&File);
}

void MPIRead(MPI_Comm Comm, std::pair<size_t, size_t> Div, std::string Name, std::vector<real> & D)
{
    MPIRead(Comm, Name, Div.first, Div.second-Div.first, D.data());
}
void MPIRead(std::pair<size_t, size_t> Div, std::string Name, std::vector<real> & D, int Mode)
{
    MPIRead(MPI_COMM_WORLD, Name, Div.first, Div.second-Div.first, D.data(), Mode);
}
void MPIRead(std::string Name, std::vector<real> & D)
{
    MPIRead(MPI_COMM_SELF, Name, 0, D.size(), D.data());
}

void MPIWrite(MPI_Comm Comm, std::string Name, MPI_Offset Offset, size_t Sz, real * D, int Mode = 0)
{
    Mode |= MPI_MODE_UNIQUE_OPEN | MPI_MODE_CREATE | MPI_MODE_WRONLY;

    MPI_Info Info = MPI_INFO_NULL;
    MPI_File File = MPI_FILE_NULL;
    int Err = MPI_File_open(Comm, Name.c_str(), Mode, Info, &File);
    if (Err != MPI_SUCCESS)
    {
        std::cout << "MPI_File failure: " << Err << std::endl;
        exit(EXIT_FAILURE);
    }
    Err = MPI_File_set_view(File, 0, MPI_DOUBLE, MPI_DOUBLE, "native", Info);
    //Err = MPI_File_set_view(File, Offset, MPI_DOUBLE, MPI_DOUBLE, "native", Info);
    if (Err != MPI_SUCCESS)
    {
        std::cout << "set MPI view failure: " << Err << std::endl;
        exit(EXIT_FAILURE);
    }
//    auto MPIWrite = [] (MPI_File f, void * d, int s, MPI_Datatype da, MPI_Status * st) { return MPI_File_write(f, d, s, da, st); };
//    MPIIO(MPIWrite, File, Sz, D); 

    auto MPIWrite = [] (MPI_File f, MPI_Offset o, void * d, int s, MPI_Datatype da, MPI_Status * st) { return MPI_File_write_at_all(f, o, d, s, da, st); };
    MPIIO(MPIWrite, File, Offset, Sz, D);
    MPI_File_close(&File);
}

void MPIWrite(MPI_Comm Comm, std::pair<size_t, size_t> Div, std::string Name, std::vector<real> & D)
{
    MPIWrite(Comm, Name, Div.first, Div.second-Div.first, D.data());
}

void MPIWrite(std::pair<size_t, size_t> Div, std::string Name, std::vector<real> & D, int Mode)
{
    MPIWrite(MPI_COMM_WORLD, Name, Div.first, Div.second-Div.first, D.data(), Mode);
}
void MPIWrite(std::string Name, std::vector<real> & D)
{
    MPIWrite(MPI_COMM_SELF, Name, 0, D.size(), D.data());
}
}

