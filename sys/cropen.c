#include "ctest.h"
#include "cfileapi.h"
#include <stddef.h>

typedef struct
{
    size_t start;
    size_t end;
} Extent;

Extent decompose(size_t work, size_t nproc, size_t rank)
{
    size_t r = work%nproc;
    size_t q = work/nproc;
    size_t start = q * rank + MIN(rank, r);
    Extent extent = {.start=start, .end=MIN(work - start, q + (rank < r))};
    return extent;
}

int testManyFiles(ExSeisHandle piol, const char * name)
{
    MPIOptions opt = { .comm=MPI_COMM_SELF, .initMPI = false };
    MPI_Comm comm;
    bool initMPI;

    //Don't go too crazy or systems won't like you.
    const size_t rnum = 10;
    const size_t fnum = 1000;
    ExSeisFile * file = calloc(fnum, sizeof(ExSeisFile));

    ExSeisFile ffile = openFile(piol, name, NULL, NULL);

    const char * msg = readText(ffile);
    size_t ln = strlen(msg);
    size_t ns = readNs(ffile);
    size_t nt = readNt(ffile);
    double inc = readInc(ffile);

    for (size_t i = 0; i < rnum; i++)
    {
        for (size_t j = 0; j < fnum; j++)
            file[j] = openFile(piol, name, NULL, NULL);

        for (size_t j = 0; j < fnum; j++)
        {
            CMP_STR(msg, readText(file[i]));
            CMP(ln, strlen(msg));
            CMP(ns, readNs(file[i]));
            CMP(nt, readNt(file[i]));
            CMP(inc, readInc(file[i]));
        }

        for (size_t j = 0; j < fnum; j++)
            closeFile(file[j]);
    }
    free(file);
    closeFile(ffile);

    return 0;
}

