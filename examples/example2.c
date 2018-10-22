///
/// Example: example2.c    {#examples_example2_c}
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
    char* opt   = "i:o:";  // TODO: uses a GNU extension
    char* iname = NULL;
    char* oname = NULL;
    for (int c = getopt(argc, argv, opt); c != -1;
         c     = getopt(argc, argv, opt)) {

        const size_t optarg_length = strlen(optarg) + 1;

        switch (c) {
            case 'i':
                // TODO: POSIX is vague about the lifetime of optarg. Next
                //       function may be unnecessary
                free(iname);
                iname = malloc(optarg_length * sizeof(char));

                strncpy(iname, optarg, optarg_length);

                break;

            case 'o':
                free(oname);
                oname = malloc(optarg_length * sizeof(char));

                strncpy(oname, optarg, optarg_length);

                break;

            default:
                fprintf(stderr, "Invalid command line arguments\n");

                break;
        }
    }
    assert(iname && oname);

    piol_exseis* piol = piol_exseis_new(exseis_verbosity_none);
    size_t rank       = piol_exseis_get_rank(piol);

    // Create a SEGY file object for input
    piol_file_read_interface* ifh = piol_file_read_segy_new(piol, iname);
    piol_exseis_assert_ok(
      piol, "Unable to create a SEGY file object for input.");

    // Create some local variables based on the input file
    size_t nt = piol_file_read_interface_read_nt(ifh);
    size_t ns = piol_file_read_interface_read_ns(ifh);

    struct exseis_Contiguous_decomposition dec =
      exseis_block_decomposition(nt, piol_exseis_get_num_rank(piol), rank);
    size_t offset = dec.global_offset;
    size_t lnt    = dec.local_size;

    // Alloc the required memory for the data we want.
    float* trace = malloc(lnt * piol_segy_segy_trace_data_size(ns));
    piol_file_trace_metadata* trhdr = piol_file_trace_metadata_new(NULL, lnt);

    // Create a SEGY file object for output
    piol_file_write_interface* ofh = piol_file_write_segy_new(piol, oname);
    piol_exseis_assert_ok(
      piol, "Unable to create a SEGY file object for output.");

    // Write the headers based on the input file.
    piol_file_write_interface_write_ns(ofh, nt);
    piol_file_write_interface_write_nt(ofh, ns);
    piol_file_write_interface_write_text(
      ofh, piol_file_read_interface_read_text(ifh));
    piol_file_write_interface_write_sample_interval(
      ofh, piol_file_read_interface_read_sample_interval(ifh));

    // Read the traces from the input file and to the output
    piol_file_read_interface_read_trace(ifh, offset, lnt, trace, trhdr);
    piol_file_write_interface_write_trace(ofh, offset, lnt, trace, trhdr);

    // Cleanup
    //
    // We want to clean everything up, because some operations are only run,
    // e.g. file writing when the classes go out of scope, i.e. are deleted.
    //
    piol_file_trace_metadata_delete(trhdr);
    free(trace);

    piol_file_read_interface_delete(ifh);
    piol_file_write_interface_delete(ofh);

    piol_exseis_delete(piol);

    free(iname);
    free(oname);

    return 0;
}
