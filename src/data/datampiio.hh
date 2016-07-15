#ifndef PIOLDATAMPIIO_INCLUDE_GUARD
#define PIOLDATAMPIIO_INCLUDE_GUARD
#include <mpi.h>
#include <memory>
#include <typeinfo>
#include "global.hh"
#include "anc/piol.hh"
#include "anc/cmpi.hh"
#include "data/data.hh"

namespace PIOL { namespace Data {
template <typename U>
using FpR = int (*)(MPI_File, MPI_Offset, void *, int, MPI_Datatype, U *);

template <typename U>
using FpW = int (*)(MPI_File, MPI_Offset, const void *, int, MPI_Datatype, U *);

struct MPIIOOpt : public Opt
{
    int mode;
    MPI_Info info;
    size_t maxSize;
    MPIIOOpt(void);
};

class MPIIO : public Interface
{
    private :
    MPI_File file;
    MPI_Comm comm;
    MPI_Info info;
    size_t maxSize;
    public :
    MPIIO(std::shared_ptr<ExSeisPIOL> piol_, std::string name_, const MPIIOOpt & opt);

    ~MPIIO(void);

    size_t getFileSz();

    void read(size_t offset, uchar * d, size_t sz);
};

}}
#endif
