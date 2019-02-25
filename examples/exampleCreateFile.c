///
/// Example: exampleCreateFile.c    {#examples_exampleCreateFile_c}
/// ============================
///
/// @todo DOCUMENT ME - Finish documenting example.
///

#include "exseisdat/piol.h"

#include <assert.h>
#include <stdlib.h>

void create_file(const char* name, size_t nt, size_t ns, size_t sample_interval)
{
    // Initialise the PIOL by creating an ExSeis object
    piol_exseis* piol = piol_exseis_new(exseis_verbosity_none);

    // Create a SEGY file object
    piol_file_write_interface* fh = piol_file_write_segy_new(piol, name);

    // Perform some decomposition (user decides how they will decompose)
    struct exseis_Contiguous_decomposition dec = exseis_block_decomposition(
        nt, piol_exseis_get_num_rank(piol), piol_exseis_get_rank(piol));

    // The offset for the local process
    size_t offset = dec.global_offset;
    // The number of traces for the local process to handle
    size_t lnt = dec.local_size;

    // Write some header parameters
    piol_file_write_interface_write_ns(fh, ns);
    piol_file_write_interface_write_nt(fh, nt);
    piol_file_write_interface_write_sample_interval(fh, sample_interval);
    piol_file_write_interface_write_text(fh, "Test file\n");

    // Set some trace parameters
    piol_file_trace_metadata* prm = piol_file_trace_metadata_new(NULL, lnt);
    for (size_t j = 0; j < lnt; j++) {
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
    assert(lnt * ns > 0);
    float* trc = calloc(lnt * ns, sizeof(float));
    for (size_t j = 0; j < lnt * ns; j++) {
        trc[j] = (float)(offset * ns + j);
    }

    // Write the traces and trace parameters
    piol_file_write_interface_write_trace(fh, offset, lnt, trc, prm);

    // Free the data
    piol_file_trace_metadata_delete(prm);
    free(trc);

    // Close the file handle and free the piol
    piol_file_write_interface_delete(fh);
    piol_exseis_delete(piol);
}


int main(void)
{
    // Set output file name, number of traces, number of samples per trace, and
    // sampling rate
    const char* name       = "CreateFileOutputC";
    size_t nt              = 8000;
    size_t ns              = 4000;
    double sample_interval = .01;

    create_file(name, nt, ns, sample_interval);
    return 0;
}
