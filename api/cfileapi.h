#include <mpi.h>
#include <stdint.h>
#include <stdbool.h>
extern "C" {
struct PIOLWrapper;
struct ExSeisFileWrapper;

typedef struct PIOLWrapper ExSeisHandle;
typedef struct ExSeisFileWrapper ExSeisFile;

////////// Options structures

typedef struct
{
    int mode;
    MPI_Info info;
    size_t maxSize;
    MPI_Comm fcomm;
} MPIIOOptions;

typedef struct
{
    MPI_Comm comm;
    bool initMPI;
} MPIOptions;

typedef struct
{
    double incFactor;
} SEGYOptions;

/////////////

typedef struct
{
    double first;
    double second;
} ccoord_t;

typedef struct
{
    int64_t first;
    int64_t second;
} cgrid_t;

typedef enum
{
    Src,
    Rcv,
    CMP
} CCoord;

typedef enum
{
    Line
} CGrid;

extern ExSeisHandle * initPIOL(size_t logLevel, MPIOptions * mpiOpt);
extern ExSeisFile * makeFile(ExSeisHandle * piol, const char * name, SEGYOptions * opt, MPIIOOptions * mpiOpt);

extern void writeText(ExSeisFile * file, const char * text);
extern void writeNs(ExSeisFile * file, size_t ns);
extern void writeNt(ExSeisFile * f, size_t nt);
extern void writeInc(ExSeisFile * f, double inc);

extern void readCoordPoint(ExSeisFile * f, CCoord item, size_t offset, size_t sz, ccoord_t * buf);
extern void readGridPoint(ExSeisFile * f, CGrid item, size_t offset, size_t sz, cgrid_t * buf);

extern void readTrace(ExSeisFile * f, size_t offset, size_t sz, float * trace);
extern void writeTrace(ExSeisFile * f, size_t offset, size_t sz, const float * trace);
}
