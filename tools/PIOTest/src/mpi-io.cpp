/* ExSeisDat IO test tool
Initial write - cathal.obroin@ichec.ie 2016 */
#include <memory>
#include <iostream>
#include <mpi.h>
#include "mpi-io.hh"
namespace io
{
void MPIErr(int Err, MPI_Status * Stat, std::string Msg)
{
    if (Err != MPI_SUCCESS)
    {
        std::cout << "MPI Err " << Err << " " << Msg;
        if (Err == MPI_ERR_IN_STATUS)
            std::cout << " MPI_Status: " << Stat->MPI_ERROR;
        std::cout << std::endl;
        exit(EXIT_FAILURE);
    }
}

template void MPIIO<real>(MPI_Fp Fn, MPI_File & File, MPI_Offset Offset, std::vector<real> & D);

MPI_File Open(MPI_Comm Comm, std::string Name, int Mode)
{
    MPI_Info Info = MPI_INFO_NULL;
    MPI_File File = MPI_FILE_NULL;
    int Err = MPI_File_open(Comm, Name.c_str(), Mode, Info, &File);
    MPIErr(Err, NULL, "MPI_File failure\n"); 
    return File;
}

}

