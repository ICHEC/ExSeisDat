#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <sys/resource.h>
#include "sglobal.h"

extern int testManyFiles(ExSeisHandle piol, const char * name);
extern int testReadWrite(ExSeisHandle piol, const char * iname, const char * oname, size_t memmax,
                         ModPrm fprm, ModTrc ftrc);

Extent decompose(size_t sz, size_t nrank, size_t rank)
{
    size_t r = sz%nrank;
    size_t q = sz/nrank;
    size_t start = q * rank + MIN(rank, r);
    Extent extent = {.start=start, .end=MIN(sz - start, q + (rank < r))};
    return extent;
}

void SourceX1600Y2400(size_t offset, TraceParam * prm)
{
    prm->src.first = 1600.0;
    prm->src.second = 2400.0;
}

void TraceLinearInc(size_t offset, size_t ns, float * trc)
{
    for (size_t i = 0; i < ns; i++)
        trc[i] = 2.0*i + offset;
}

//TODO: Use argp
int main(int argc, char ** argv)
{
    if (argc < 4)
        return -1;
    ExSeisHandle piol = initPIOL(0, NULL);
    isErr(piol);

    size_t rank = getRank(piol);
    size_t numRank = getNumRank(piol);
    if (rank == 0)
        printf("NumRank: %zu\n", numRank);

    switch (strtol(argv[1], NULL, 10))
    {
        case 1 :
        {
            if (!getRank(piol))
                printf("Many Files test.\n");
            testManyFiles(piol, "/ichec/work/exseisdat/dat/sortedfile.segy");
        } break;
        case 2 :
        {
            if (!getRank(piol))
                printf("Read/Write test from %s to %s\n", argv[2], argv[3]);
            testReadWrite(piol, argv[2], argv[3], 2500LU, NULL, NULL);
        } break;
        case 3 :
        {
            if (!getRank(piol))
                printf("Read/Write test (trace param mod) from %s to %s\n", argv[2], argv[3]);
            testReadWrite(piol, argv[2], argv[3], 2500LU, SourceX1600Y2400, NULL);
        } break;
        case 4 :
        {
            if (!getRank(piol))
                printf("Read/Write test (trace mod) from %s to %s\n", argv[2], argv[3]);
            testReadWrite(piol, argv[2], argv[3], 2500LU, NULL, TraceLinearInc);
        } break;
        default :
            fprintf(stderr, "Unknown test\n");
    }

    closePIOL(piol);
    return 0;
}

