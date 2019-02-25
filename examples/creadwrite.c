///
/// Example: creadwrite.c    {#examples_creadwrite_c}
/// =====================
///
/// @todo DOCUMENT ME - Finish documenting example.
///
/// This file reads from an existing file, interprets it by the PIOL and writes
/// out a new file. Since the input is interpreted by the PIOL it does not
/// produce an identical copy.
///

#include "exseisdat/piol.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef void (*ModTrc)(size_t, size_t, float*);
typedef void (*ModPrm)(size_t, size_t, piol_file_trace_metadata*);

size_t max(size_t a, size_t b)
{
    if (a > b) {
        return a;
    }
    return b;
}

void read_write_full_trace(
    piol_exseis* piol,
    piol_file_read_interface* ifh,
    piol_file_write_interface* ofh,
    size_t off,
    size_t tcnt,
    ModTrc ftrc,
    ModPrm fprm)
{
    piol_file_trace_metadata* trhdr = piol_file_trace_metadata_new(NULL, tcnt);
    size_t ns                       = piol_file_read_interface_read_ns(ifh);

    assert(tcnt * piol_segy_segy_trace_data_size(ns) > 0);
    float* trace = malloc(tcnt * piol_segy_segy_trace_data_size(ns));
    assert(trace);

    piol_file_read_interface_read_trace(ifh, off, tcnt, trace, trhdr);
    if (ftrc != NULL) {
        for (size_t i = 0; i < tcnt; i++) {
            ftrc(off, ns, &trace[i]);
        }
    }

    if (fprm != NULL) {
        for (size_t i = 0; i < tcnt; i++) {
            fprm(off, i, trhdr);
        }
    }

    piol_file_write_interface_write_trace(ofh, off, tcnt, trace, trhdr);
    piol_exseis_assert_ok(piol, NULL);

    free(trace);
    piol_file_trace_metadata_delete(trhdr);
}

void write_payload(
    piol_exseis* piol,
    piol_file_read_interface* ifh,
    piol_file_write_interface* ofh,
    size_t goff,
    size_t lnt,
    size_t tcnt,
    ModPrm fprm,
    ModTrc ftrc)
{
    size_t biggest = lnt;
    MPI_Allreduce(
        &lnt, &biggest, 1, MPI_UNSIGNED_LONG, MPI_MAX, MPI_COMM_WORLD);
    size_t extra =
        biggest / tcnt - lnt / tcnt + (biggest % tcnt > 0) - (lnt % tcnt > 0);

    for (size_t i = 0U; i < lnt; i += tcnt) {
        read_write_full_trace(
            piol, ifh, ofh, goff + i, (i + tcnt < lnt ? tcnt : lnt - i), ftrc,
            fprm);
    }

    for (size_t i = 0U; i < extra; i++) {
        read_write_full_trace(piol, ifh, ofh, goff, 0, ftrc, fprm);
    }
}

int read_write_file(
    piol_exseis* piol,
    const char* iname,
    const char* oname,
    size_t memmax,
    ModPrm fprm,
    ModTrc ftrc)
{
    piol_file_read_interface* ifh = piol_file_read_segy_new(piol, iname);
    piol_exseis_assert_ok(piol, NULL);

    size_t ns = piol_file_read_interface_read_ns(ifh);
    size_t nt = piol_file_read_interface_read_nt(ifh);
    // Write all header metadata
    piol_file_write_interface* ofh = piol_file_write_segy_new(piol, oname);
    piol_exseis_assert_ok(piol, NULL);

    piol_file_write_interface_write_text(
        ofh, piol_file_read_interface_read_text(ifh));
    piol_file_write_interface_write_ns(
        ofh, piol_file_read_interface_read_ns(ifh));
    piol_file_write_interface_write_nt(
        ofh, piol_file_read_interface_read_nt(ifh));
    piol_file_write_interface_write_sample_interval(
        ofh, piol_file_read_interface_read_sample_interval(ifh));
    piol_exseis_assert_ok(piol, NULL);

    struct exseis_Contiguous_decomposition dec = exseis_block_decomposition(
        nt, piol_exseis_get_num_rank(piol), piol_exseis_get_rank(piol));
    size_t tcnt = memmax
                  / max(
                      piol_segy_segy_trace_data_size(ns),
                      piol_segy_segy_trace_header_size());

    write_payload(
        piol, ifh, ofh, dec.global_offset, dec.local_size, tcnt, fprm, ftrc);

    piol_exseis_assert_ok(piol, NULL);
    piol_file_write_interface_delete(ofh);
    piol_file_read_interface_delete(ifh);
    return 0;
}

void source_x1600_y2400(size_t offset, size_t i, piol_file_trace_metadata* prm)
{
    piol_file_set_prm_double(i, exseis_meta_x_src, offset + 1600.0, prm);
    piol_file_set_prm_double(i, exseis_meta_y_src, offset + 2400.0, prm);
}

void trace_linear_interval(size_t offset, size_t ns, float* trc)
{
    for (size_t i = 0; i < ns; i++) {
        trc[i] = 2.0 * i + offset;
    }
}


int main(int argc, char** argv)
{
    //  Flags:
    // -i input file
    // -o output file
    // -m maximum memory
    // -p modify trace paramaters
    // -t modify trace values
    char* opt = "i:o:m:pt";  // TODO: uses a GNU extension

    char* iname = NULL;
    char* oname = NULL;

    size_t memmax = 2U * 1024U * 1024U * 1024U;  // bytes

    ModPrm mod_prm = NULL;
    ModTrc mod_trc = false;

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

            case 'm':
                if (sscanf(optarg, "%zu", &memmax) != 1) {
                    fprintf(stderr, "Incorrect arguments to memmax option\n");

                    free(iname);
                    free(oname);

                    return -1;
                }

                break;

            case 'p':
                printf("The trace parameters will be modified\n");
                mod_prm = source_x1600_y2400;

                break;

            case 't':
                printf("The traces will be modified\n");
                mod_trc = trace_linear_interval;

                break;

            default:
                fprintf(
                    stderr, "One of the command line arguments is invalid\n");

                break;
        }
    }
    assert(iname && oname);

    piol_exseis* piol = piol_exseis_new(exseis_verbosity_none);
    piol_exseis_assert_ok(piol, NULL);

    read_write_file(piol, iname, oname, memmax, mod_prm, mod_trc);

    piol_exseis_delete(piol);

    free(iname);
    free(oname);

    return 0;
}
