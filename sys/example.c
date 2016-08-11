#include "cfileapi.h"
#include <stddef.h>
#include <stdio.h>
#define MIN(x, y) (x < y ? x : y)

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

int main(int argc, char ** argv)
{
    if (argc < 2)
        return -1;

    ExSeisHandle piol = initPIOL(0, NULL);
    isErr(piol);

    printf("Initialised PIOL\n");
    size_t rank = getRank(piol);
    size_t numRank = getNumRank(piol);

    printf("Process %zu of %zu\n", rank, numRank);

    ExSeisFile file = openFile(piol, argv[1], NULL, NULL);

    const char * msg = readText(file);
    size_t ns = readNs(file);
    size_t nt = readNt(file);
    double inc = readInc(file);

    printf("The text header says %s traces.\n", msg);
    printf("There are %zu traces.\n", nt);
    printf("There are %zu samples per traces.\n", ns);
    printf("The increment between traces is %e.\n", inc);

    closeFile(file);
    closePIOL(piol);
}

