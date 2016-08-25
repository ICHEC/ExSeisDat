#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <sys/resource.h>
#include "sglobal.h"
#include "cfileapi.h"

extern int testManyFiles(ExSeisHandle piol, const char * name);
extern int testReadWrite(ExSeisHandle piol, const char * iname, const char * oname, size_t bsz);

Extent decompose(size_t sz, size_t nrank, size_t rank)
{
    size_t r = sz%nrank;
    size_t q = sz/nrank;
    size_t start = q * rank + MIN(rank, r);
    Extent extent = {.start=start, .end=MIN(sz - start, q + (rank < r))};
    return extent;
}
//TODO: Use argp
int main(int argc, char ** argv)
{
    if (argc < 3)
        return -1;
    ExSeisHandle piol = initPIOL(0, NULL);
    isErr(piol);

    size_t rank = getRank(piol);
    size_t numRank = getNumRank(piol);
    if (rank == 0)
        printf("NumRank: %zu\n", numRank);

//    testManyFiles(piol, "/ichec/work/exseisdat/dat/sortedfile.segy");
    testReadWrite(piol, argv[1], argv[2], 2500LU);

    //testReadWrite(piol, "dat/rtm_salt3d_data_shot.segy", "dat/test.segy", 2500LU);
//    testReadWrite(piol, "dat/sortedfile.segy", "dat/test.segy", 2500LU);

    closePIOL(piol);
    return 0;
}

