#ifndef MPIIO_GUARD
#define MPIIO_GUARD
#include <mpi.h>
#include <vector>
typedef double real;
namespace io
{
extern MPI_File MPIOpen(MPI_Comm Comm, std::string Name, int Mode);
typedef int (* MPI_Fp)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, MPI_Status *);
extern void MPIIO(MPI_Fp Fn, MPI_File & File, MPI_Offset Offset, MPI_Offset Sz, real * D);
extern void MPIRead(MPI_Comm, std::pair<MPI_Offset, MPI_Offset> &, std::string, std::vector<real> &, int Mode = 0);
extern void MPIWrite(MPI_Comm, std::pair<MPI_Offset, MPI_Offset> &, std::string, std::vector<real> &, int Mode = 0);
}
#endif
