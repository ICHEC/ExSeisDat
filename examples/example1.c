///
/// Example: example1.c    {#examples_example1_c}
/// ===================
///
/// @todo DOCUMENT ME - Finish documenting example.
///

#include "ExSeisDat/PIOL.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    // Initialise the PIOL by creating an ExSeis object
    PIOL_ExSeis* piol = PIOL_ExSeis_new(PIOL_VERBOSITY_NONE);

    // Parse command line options
    //      -o OUTPUT_NAME
    char* opt  = "o:";  // TODO: uses a GNU extension
    char* name = NULL;
    for (int c = getopt(argc, argv, opt); c != -1;
         c     = getopt(argc, argv, opt)) {

        const size_t optarg_length = strlen(optarg) + 1;

        if (c == 'o') {
            free(name);
            name = malloc(optarg_length * sizeof(char));

            strncpy(name, optarg, optarg_length);
        }
        else {
            fprintf(stderr, "One of the command line arguments is invalid\n");
        }
    }
    assert(name);

    // Create a SEGY file object
    PIOL_File_WriteDirect* fh = PIOL_File_WriteDirect_new(piol, name);


    // The number of traces
    size_t nt = 40000;

    // The number of samples per trace
    size_t ns = 1000;

    // The increment step between traces (microseconds)
    double inc = 4.0;


    // Perform some decomposition (user decides how they will decompose)

    // Get number of process and current process number
    size_t num_ranks = PIOL_ExSeis_getNumRank(piol);
    size_t rank      = PIOL_ExSeis_getRank(piol);

    // Get decomposition of the range [0..nt) for the current rank
    struct PIOL_Contiguous_decomposition dec =
      PIOL_block_decompose(nt, num_ranks, rank);

    // The offset for the local process
    size_t offset = dec.global_offset;

    // The number of traces for the local process to handle
    size_t local_nt = dec.local_size;


    // Write some global header parameters
    PIOL_File_WriteDirect_writeNs(fh, ns);
    PIOL_File_WriteDirect_writeNt(fh, nt);
    PIOL_File_WriteDirect_writeInc(fh, inc);
    PIOL_File_WriteDirect_writeText(fh, "Test file\n");

    // Generate parameter structure for the local traces
    PIOL_File_Param* prm = PIOL_File_Param_new(NULL, local_nt);

    // Set some trace parameters
    for (size_t j = 0; j < local_nt; j++) {
        float k = offset + j;

        PIOL_File_setPrm_double(j, PIOL_META_xSrc, 1600.0 + k, prm);
        PIOL_File_setPrm_double(j, PIOL_META_ySrc, 2400.0 + k, prm);

        PIOL_File_setPrm_double(j, PIOL_META_xRcv, 100000.0 + k, prm);
        PIOL_File_setPrm_double(j, PIOL_META_yRcv, 3000000.0 + k, prm);

        PIOL_File_setPrm_double(j, PIOL_META_xCmp, 10000.0 + k, prm);
        PIOL_File_setPrm_double(j, PIOL_META_yCmp, 4000.0 + k, prm);

        PIOL_File_setPrm_llint(j, PIOL_META_il, 2400 + offset + j, prm);
        PIOL_File_setPrm_llint(j, PIOL_META_xl, 1600 + offset + j, prm);

        PIOL_File_setPrm_llint(j, PIOL_META_tn, offset + j, prm);
    }

    // Set some traces
    assert(local_nt * ns > 0);
    float* trc = calloc(local_nt * ns, sizeof(float));
    for (size_t j = 0; j < local_nt * ns; j++) {
        trc[j] = (float)(offset * ns + j);
    }

    // Write the traces and trace parameters
    PIOL_File_WriteDirect_writeTrace(fh, offset, local_nt, trc, prm);

    // Free the data
    PIOL_File_Param_delete(prm);
    free(trc);

    // Free/close the file handle and free the piol
    PIOL_File_WriteDirect_delete(fh);
    PIOL_ExSeis_delete(piol);

    free(name);

    return 0;
}
