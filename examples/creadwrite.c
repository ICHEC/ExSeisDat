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

#include "ExSeisDat/PIOL.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef void (*ModTrc)(size_t, size_t, float*);
typedef void (*ModPrm)(size_t, size_t, PIOL_File_Param*);

size_t max(size_t a, size_t b)
{
    if (a > b) {
        return a;
    }
    return b;
}

void readWriteFullTrace(
  PIOL_ExSeis* piol,
  PIOL_File_ReadDirect* ifh,
  PIOL_File_WriteDirect* ofh,
  size_t off,
  size_t tcnt,
  ModTrc ftrc,
  ModPrm fprm)
{
    PIOL_File_Param* trhdr = PIOL_File_Param_new(NULL, tcnt);
    size_t ns              = PIOL_File_ReadDirect_readNs(ifh);

    assert(tcnt * PIOL_SEGY_utils_getDFSz(ns) > 0);
    float* trace = malloc(tcnt * PIOL_SEGY_utils_getDFSz(ns));
    assert(trace);

    PIOL_File_ReadDirect_readTrace(ifh, off, tcnt, trace, trhdr);
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

    PIOL_File_WriteDirect_writeTrace(ofh, off, tcnt, trace, trhdr);
    PIOL_ExSeis_isErr(piol, NULL);

    free(trace);
    PIOL_File_Param_delete(trhdr);
}

void writePayload(
  PIOL_ExSeis* piol,
  PIOL_File_ReadDirect* ifh,
  PIOL_File_WriteDirect* ofh,
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

    for (size_t i = 0U; i < lnt; i += tcnt)
        readWriteFullTrace(
          piol, ifh, ofh, goff + i, (i + tcnt < lnt ? tcnt : lnt - i), ftrc,
          fprm);

    for (size_t i = 0U; i < extra; i++)
        readWriteFullTrace(piol, ifh, ofh, goff, 0, ftrc, fprm);
}

int ReadWriteFile(
  PIOL_ExSeis* piol,
  const char* iname,
  const char* oname,
  size_t memmax,
  ModPrm fprm,
  ModTrc ftrc)
{
    PIOL_File_ReadDirect* ifh = PIOL_File_ReadDirect_new(piol, iname);
    PIOL_ExSeis_isErr(piol, NULL);

    size_t ns = PIOL_File_ReadDirect_readNs(ifh);
    size_t nt = PIOL_File_ReadDirect_readNt(ifh);
    // Write all header metadata
    PIOL_File_WriteDirect* ofh = PIOL_File_WriteDirect_new(piol, oname);
    PIOL_ExSeis_isErr(piol, NULL);

    PIOL_File_WriteDirect_writeText(ofh, PIOL_File_ReadDirect_readText(ifh));
    PIOL_File_WriteDirect_writeNs(ofh, PIOL_File_ReadDirect_readNs(ifh));
    PIOL_File_WriteDirect_writeNt(ofh, PIOL_File_ReadDirect_readNt(ifh));
    PIOL_File_WriteDirect_writeInc(ofh, PIOL_File_ReadDirect_readInc(ifh));
    PIOL_ExSeis_isErr(piol, NULL);

    struct PIOL_Range dec = PIOL_decompose(
      nt, PIOL_ExSeis_getNumRank(piol), PIOL_ExSeis_getRank(piol));
    size_t tcnt =
      memmax / max(PIOL_SEGY_utils_getDFSz(ns), PIOL_SEGY_utils_getMDSz());

    writePayload(piol, ifh, ofh, dec.offset, dec.size, tcnt, fprm, ftrc);

    PIOL_ExSeis_isErr(piol, NULL);
    PIOL_File_WriteDirect_delete(ofh);
    PIOL_File_ReadDirect_delete(ifh);
    return 0;
}

void SourceX1600Y2400(size_t offset, size_t i, PIOL_File_Param* prm)
{
    PIOL_File_setPrm_double(i, PIOL_META_xSrc, offset + 1600.0, prm);
    PIOL_File_setPrm_double(i, PIOL_META_ySrc, offset + 2400.0, prm);
}

void TraceLinearInc(size_t offset, size_t ns, float* trc)
{
    for (size_t i = 0; i < ns; i++)
        trc[i] = 2.0 * i + offset;
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

    ModPrm modPrm = NULL;
    ModTrc modTrc = false;

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
                modPrm = SourceX1600Y2400;

                break;

            case 't':
                printf("The traces will be modified\n");
                modTrc = TraceLinearInc;

                break;

            default:
                fprintf(
                  stderr, "One of the command line arguments is invalid\n");

                break;
        }
    }
    assert(iname && oname);

    PIOL_ExSeis* piol = PIOL_ExSeis_new(PIOL_VERBOSITY_NONE);
    PIOL_ExSeis_isErr(piol, NULL);

    ReadWriteFile(piol, iname, oname, memmax, modPrm, modTrc);

    PIOL_ExSeis_delete(piol);

    free(iname);
    free(oname);

    return 0;
}
