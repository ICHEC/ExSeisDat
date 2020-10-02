/// @todo Move this to spectests
#include "exseisdat/piol.h"

#include <assert.h>
#include <stddef.h>
#include <unistd.h>

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void c_test_quit(const char* msg, size_t line);

#define MIN(x, y) (x < y ? x : y)
#define EXSEISDAT_MAX(x, y) (x > y ? x : y)
#define EXSEISDAT_CMP(X, Y)                                                    \
    {                                                                          \
        size_t line = __LINE__;                                                \
        if (X != Y) {                                                          \
            CTest_quit("Failure in ## __FILE__ ## Line %zu\n", line);          \
        }                                                                      \
    }

#define EXSEISDAT_CMP_STR(X, Y)                                                \
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
    if (!rank) {
        fprintf(stderr, msg, line);
    }
    MPI_Finalize();
    exit(-1);
}

int test_many_files(piol_exseis* piol, const char* name)
{
    // Don't go too crazy or systems won't like you.
    const size_t rnum = 10;
    const size_t fnum = 1000;
    piol_file_read_interface** files =
        calloc(fnum, sizeof(piol_file_read_interface*));
    piol_file_read_interface* ffile = piol_file_read_segy_new(piol, name);

    const char* msg = piol_file_read_interface_read_text(ffile);
    size_t ln       = strlen(msg);
    size_t ns       = piol_file_read_interface_read_ns(ffile);
    size_t nt       = piol_file_read_interface_read_nt(ffile);
    double inc      = piol_file_read_interface_read_sample_interval(ffile);

    for (size_t i = 0; i < rnum; i++) {
        for (size_t j = 0; j < fnum; j++) {
            files[j] = piol_file_read_segy_new(piol, name);
        }

        for (size_t j = 0; j < fnum; j++) {
            EXSEISDAT_CMP_STR(
                msg, piol_file_read_interface_read_text(files[i]));
            EXSEISDAT_CMP(ln, strlen(msg));
            EXSEISDAT_CMP(ns, piol_file_read_interface_read_ns(files[i]));
            EXSEISDAT_CMP(nt, piol_file_read_interface_read_nt(files[i]));
            EXSEISDAT_CMP(
                inc, piol_file_read_interface_read_sample_interval(files[i]));
        }

        for (size_t j = 0; j < fnum; j++) {
            piol_file_read_interface_delete(files[j]);
        }
    }

    piol_file_read_interface_delete(ffile);
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

    piol_exseis* piol = piol_exseis_new(exseis_verbosity_none);
    piol_exseis_assert_ok(piol, "");

    test_many_files(piol, name);

    free(name);

    return 0;
}
