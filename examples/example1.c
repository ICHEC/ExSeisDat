///
/// Example: example1.c    {#examples_example1_c}
/// ===================
///
/// @todo DOCUMENT ME - Finish documenting example.
///

#include "exseisdat/piol.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    // Initialise the PIOL by creating an ExSeis object
    piol_exseis* piol = piol_exseis_new(exseis_verbosity_none);

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
    piol_file_write_interface* fh = piol_file_write_segy_new(piol, name);


    // The number of traces
    size_t nt = 40000;

    // The number of samples per trace
    size_t ns = 1000;

    // The increment step between traces (microseconds)
    double sample_interval = 4.0;


    // Perform some decomposition (user decides how they will decompose)

    // Get number of process and current process number
    size_t num_ranks = piol_exseis_get_num_rank(piol);
    size_t rank      = piol_exseis_get_rank(piol);

    // Get decomposition of the range [0..nt) for the current rank
    struct exseis_Contiguous_decomposition dec =
        exseis_block_decomposition(nt, num_ranks, rank);

    // The offset for the local process
    size_t offset = dec.global_offset;

    // The number of traces for the local process to handle
    size_t local_nt = dec.local_size;


    // Write some global header parameters
    piol_file_write_interface_write_ns(fh, ns);
    piol_file_write_interface_write_nt(fh, nt);
    piol_file_write_interface_write_sample_interval(fh, sample_interval);
    piol_file_write_interface_write_text(fh, "Test file\n");

    // Generate parameter structure for the local traces
    piol_file_trace_metadata* prm =
        piol_file_trace_metadata_new(NULL, local_nt);

    // Set some trace parameters
    for (size_t j = 0; j < local_nt; j++) {
        float k = offset + j;

        piol_file_set_prm_double(j, exseis_meta_x_src, 1600.0 + k, prm);
        piol_file_set_prm_double(j, exseis_meta_y_src, 2400.0 + k, prm);

        piol_file_set_prm_double(j, exseis_meta_x_rcv, 100000.0 + k, prm);
        piol_file_set_prm_double(j, exseis_meta_y_rcv, 3000000.0 + k, prm);

        piol_file_set_prm_double(j, exseis_meta_xCmp, 10000.0 + k, prm);
        piol_file_set_prm_double(j, exseis_meta_yCmp, 4000.0 + k, prm);

        piol_file_set_prm_integer(j, exseis_meta_il, 2400 + offset + j, prm);
        piol_file_set_prm_integer(j, exseis_meta_xl, 1600 + offset + j, prm);

        piol_file_set_prm_integer(j, exseis_meta_tn, offset + j, prm);
    }

    // Set some traces
    assert(local_nt * ns > 0);
    float* trc = calloc(local_nt * ns, sizeof(float));
    for (size_t j = 0; j < local_nt * ns; j++) {
        trc[j] = (float)(offset * ns + j);
    }

    // Write the traces and trace parameters
    piol_file_write_interface_write_trace(fh, offset, local_nt, trc, prm);

    // Free the data
    piol_file_trace_metadata_delete(prm);
    free(trc);

    // Free/close the file handle and free the piol
    piol_file_write_interface_delete(fh);
    piol_exseis_delete(piol);

    free(name);

    return 0;
}
