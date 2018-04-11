/// @todo Move this to spectests
#include "ExSeisDat/PIOL.h"

#include <assert.h>
#include <stddef.h>
#include <unistd.h>

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void CTest_quit(const char* msg, size_t line);

#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)
#define CMP(X, Y)                                                              \
    {                                                                          \
        size_t line = __LINE__;                                                \
        if (X != Y) {                                                          \
            CTest_quit("Failure in ## __FILE__ ## Line %zu\n", line);          \
        }                                                                      \
    }

#define CMP_STR(X, Y)                                                          \
    {                                                                          \
        size_t line = __LINE__;                                                \
        size_t len1 = strlen(X), len2 = strlen(Y);                             \
        if (strncmp(X, Y, MIN(len1, len2))) {                                  \
            CTest_quit("Failure in ## __FILE__ ## Line %zu\n", line);          \
        }                                                                      \
    }

void CTest_quit(const char* msg, size_t line)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (!rank) fprintf(stderr, msg, line);
    MPI_Finalize();
    exit(-1);
}

int testManyFiles(PIOL_ExSeis* piol, const char* name)
{
    // Don't go too crazy or systems won't like you.
    const size_t rnum            = 10;
    const size_t fnum            = 1000;
    PIOL_File_ReadDirect** files = calloc(fnum, sizeof(PIOL_File_ReadDirect*));
    PIOL_File_ReadDirect* ffile  = PIOL_File_ReadDirect_new(piol, name);

    const char* msg = PIOL_File_ReadDirect_readText(ffile);
    size_t ln       = strlen(msg);
    size_t ns       = PIOL_File_ReadDirect_readNs(ffile);
    size_t nt       = PIOL_File_ReadDirect_readNt(ffile);
    double inc      = PIOL_File_ReadDirect_readInc(ffile);

    for (size_t i = 0; i < rnum; i++) {
        for (size_t j = 0; j < fnum; j++) {
            files[j] = PIOL_File_ReadDirect_new(piol, name);
        }

        for (size_t j = 0; j < fnum; j++) {
            CMP_STR(msg, PIOL_File_ReadDirect_readText(files[i]));
            CMP(ln, strlen(msg));
            CMP(ns, PIOL_File_ReadDirect_readNs(files[i]));
            CMP(nt, PIOL_File_ReadDirect_readNt(files[i]));
            CMP(inc, PIOL_File_ReadDirect_readInc(files[i]));
        }

        for (size_t j = 0; j < fnum; j++) {
            PIOL_File_ReadDirect_delete(files[j]);
        }
    }

    PIOL_File_ReadDirect_delete(ffile);
    free(files);

    return 0;
}

int main(int argc, char** argv)
{
    //  Flags:
    // -i input file
    // -o output file
    // -m maximum memory
    char* opt  = "i:";  // TODO: uses a GNU extension
    char* name = NULL;
    for (int c = getopt(argc, argv, opt); c != -1;
         c     = getopt(argc, argv, opt)) {
        if (c == 'i') {
            const size_t optarg_len = strlen(optarg) + 1;
            free(name);
            name = malloc(optarg_len * sizeof(char));
            strncpy(name, optarg, optarg_len);
        }
        else {
            fprintf(stderr, "One of the command line arguments is invalid\n");

            free(name);
            return -1;
        }
    }
    assert(name);

    PIOL_ExSeis* piol = PIOL_ExSeis_new(PIOL_VERBOSITY_NONE);
    PIOL_ExSeis_isErr(piol, "");

    testManyFiles(piol, name);

    free(name);

    return 0;
}
