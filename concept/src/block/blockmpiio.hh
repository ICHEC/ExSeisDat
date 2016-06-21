#ifndef PIOLBLOCKMPI_INCLUDE_GUARD
#define PIOLBLOCKMPI_INCLUDE_GUARD
#include <mpi.h>
#include <memory>
#include "global.hh"
#include "comm/mpi.hh"
#include "block/block.hh"

namespace PIOL { namespace Block {
template <typename U>
using Fp = int (*)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, U *);
extern int mpiio_write_at(MPI_File f, MPI_Offset o, void * d, int s, MPI_Datatype da, MPI_Status * st);
class MPIIO : public PIOL::Block::Interface
{
#ifdef TEST_PRIVATE
    public :
#endif

    MPI_File file;
    Fp<MPI_Status> ifn;
    Fp<MPI_Status> ofn;
    MPI_Comm mcomm;

#ifndef TEST_PRIVATE
    public :
#endif
    MPIIO(std::shared_ptr<Comms::MPI> Comm, std::string name, 
          Fp<MPI_Status> Ifn = MPI_File_read_at,
          Fp<MPI_Status> Ofn = mpiio_write_at);
    MPIIO(std::shared_ptr<Comms::MPI> Comm, std::string name, int mode, 
          Fp<MPI_Status> Ifn = MPI_File_read_at,
          Fp<MPI_Status> Ofn = mpiio_write_at); 

    ~MPIIO(void);

    size_t getFileSz();
    void setFileSz(size_t sz);
    void setView(size_t offset = 0);
    void readData(size_t o, float * f, size_t s);
    void readData(size_t o, uchar * c, size_t s);
    void writeData(size_t o, float * f, size_t s);
    void writeData(size_t o, uchar * c, size_t s);
};
}}
#endif
