#include <mpi.h>
#include <memory>
#include <iostream>
#include <mpi.h>
#include "mpiio.hh"

namespace PIOL { namespace Block { namespace MPI {
void printErr(int err, MPI_Status * stat, std::string msg)
{
    if (err != MPI_SUCCESS)
    {
        std::cout << "MPI Err " << err << " " << msg;
        if (err == MPI_ERR_IN_STATUS)
            std::cout << " MPI_Status: " << stat->MPI_ERROR;
        std::cout << std::endl;
        exit(EXIT_FAILURE);
    }
}
size_t getFileSz(MPI_File file)
{
    MPI_Offset fsz;
    int err = MPI_File_get_size(file, &fsz);
    printErr(err, NULL, "error getting the file size");
    return size_t(fsz);
}

void setFileSz(MPI_File file, size_t sz)
{
    int err = MPI_File_set_size(file, sz);
    Block::MPI::printErr(err, NULL, "Error resizing file\n");
}
//template void MPIIO<real>(MPI_Fp, MPI_File & File, MPI_Offset Offset, std::vector<real> & D);

MPI_File open(MPI_Comm comm, std::string name, int mode)
{
    MPI_Info info = MPI_INFO_NULL;
    MPI_File file = MPI_FILE_NULL;
    int err = MPI_File_open(comm, name.c_str(), mode, info, &file);
    printErr(err, NULL, "MPI_File failure " + name + " \n");
    return file;
}
}}}

