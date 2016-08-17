#include "cfileapi.h"
#include <stddef.h>
#include <stdio.h>

#include <sys/resource.h>

extern int testManyFiles(ExSeisHandle piol, const char * name);

int main(int argc, char ** argv)
{
    ExSeisHandle piol = initPIOL(0, NULL);
    isErr(piol);

    size_t rank = getRank(piol);
    size_t numRank = getNumRank(piol);
    if (rank == 0)
        printf("NumRank: %zu\n", numRank);

    testManyFiles(piol, "/ichec/work/exseisdat/dat/sortedfile.segy");

    closePIOL(piol);
    return 0;
}

