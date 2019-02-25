///
/// Example: modprm.c    {#examples_modprm_c}
/// =================
///
/// @todo DOCUMENT ME - Finish documenting example.
///

//#include "ctest.h"

#include "exseisdat/piol.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

size_t max(size_t a, size_t b)
{
    if (a > b) {
        return a;
    }
    return b;
}

/*! Read nt parameters from the input file ifh and write it to
 *  the output file ofh. Parameters are read and written from
 *  the offset off.
 *  @param[in] piol The PIOL handle
 *  @param[in] off The offset in number of traces.
 *  @param[in] tcnt The number of traces.
 *  @param[in] ifh The input file handle
 *  @param[out] ofh The output file handle
 */
void readwrite_param(
    piol_exseis* piol,
    size_t off,
    size_t tcnt,
    piol_file_read_interface* ifh,
    piol_file_write_interface* ofh)
{
    piol_file_trace_metadata* prm = piol_file_trace_metadata_new(NULL, tcnt);
    piol_file_read_interface_read_param(ifh, off, tcnt, prm);
    for (size_t i = 0; i < tcnt; i++) {
        exseis_Floating_point xval =
            piol_file_get_prm_double(i, exseis_meta_x_src, prm);
        exseis_Floating_point yval =
            piol_file_get_prm_double(i, exseis_meta_y_src, prm);
        piol_file_set_prm_double(i, exseis_meta_x_src, yval, prm);
        piol_file_set_prm_double(i, exseis_meta_y_src, xval, prm);
    }

    piol_file_write_interface_write_param(ofh, off, tcnt, prm);
    piol_exseis_assert_ok(piol, "");
    piol_file_trace_metadata_delete(prm);
}

/*! Write the output header details.
 *  @param[in] piol The PIOL handle
 *  @param[in] ifh The input file handle
 *  @param[out] ofh The output file handle
 */
void write_header(
    piol_exseis* piol,
    piol_file_read_interface* ifh,
    piol_file_write_interface* ofh)
{
    piol_file_write_interface_write_text(
        ofh, piol_file_read_interface_read_text(ifh));
    piol_file_write_interface_write_ns(
        ofh, piol_file_read_interface_read_ns(ifh));
    piol_file_write_interface_write_nt(
        ofh, piol_file_read_interface_read_nt(ifh));
    piol_file_write_interface_write_sample_interval(
        ofh, piol_file_read_interface_read_sample_interval(ifh));
    piol_exseis_assert_ok(piol, "");
}

/*! Write the data from the input file to the output file
 *  @param[in] piol The PIOL handle
 *  @param[in] goff The global offset in number of traces.
 *  @param[in] lnt The lengthof each trace.
 *  @param[in] tcnt The number of traces.
 *  @param[in] ifh The input file handle
 *  @param[out] ofh The output file handle
 */
void write_payload(
    piol_exseis* piol,
    size_t goff,
    size_t lnt,
    size_t tcnt,
    piol_file_read_interface* ifh,
    piol_file_write_interface* ofh)
{
    size_t biggest = lnt;
    MPI_Allreduce(
        &lnt, &biggest, 1, MPI_UNSIGNED_LONG, MPI_MAX, MPI_COMM_WORLD);
    size_t extra =
        biggest / tcnt - lnt / tcnt + (biggest % tcnt > 0) - (lnt % tcnt > 0);

    for (size_t i = 0U; i < lnt; i += tcnt) {
        size_t rblock = (i + tcnt < lnt ? tcnt : lnt - i);
        readwrite_param(piol, goff + i, rblock, ifh, ofh);
    }

    for (size_t i = 0U; i < extra; i++) {
        readwrite_param(piol, goff, 0, ifh, ofh);
    }
}

int main(int argc, char** argv)
{
    //  Flags:
    // -i input file
    // -o output file
    // -m maximum memory
    char* opt     = "i:o:";  // TODO: uses a GNU extension
    char* iname   = NULL;
    char* oname   = NULL;
    size_t memmax = 2U * 1024U * 1024U * 1024U;  // bytes
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
                fprintf(
                    stderr, "One of the command line arguments is invalid\n");

                break;
        }
    }
    assert(iname && oname);

    piol_exseis* piol = piol_exseis_new(exseis_verbosity_none);
    piol_exseis_assert_ok(piol, "");

    piol_file_read_interface* ifh = piol_file_read_segy_new(piol, iname);
    piol_exseis_assert_ok(piol, "");

    size_t ns = piol_file_read_interface_read_ns(ifh);
    size_t nt = piol_file_read_interface_read_nt(ifh);
    // Write all header metadata
    piol_file_write_interface* ofh = piol_file_write_segy_new(piol, oname);
    piol_exseis_assert_ok(piol, "");

    write_header(piol, ifh, ofh);

    struct exseis_Contiguous_decomposition dec = exseis_block_decomposition(
        nt, piol_exseis_get_num_rank(piol), piol_exseis_get_rank(piol));
    size_t tcnt = memmax
                  / max(
                      piol_segy_segy_trace_data_size(ns),
                      piol_segy_segy_trace_header_size());

    write_payload(piol, dec.global_offset, dec.local_size, tcnt, ifh, ofh);

    piol_exseis_assert_ok(piol, "");
    piol_file_write_interface_delete(ofh);
    piol_file_read_interface_delete(ifh);
    piol_exseis_delete(piol);

    free(iname);
    free(oname);

    return 0;
}
