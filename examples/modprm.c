///
/// Example: modprm.c    {#examples_modprm_c}
/// =================
///
/// @todo DOCUMENT ME - Finish documenting example.
///

//#include "ctest.h"

#include "ExSeisDat/PIOL.h"

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
void readwriteParam(
  PIOL_ExSeis* piol,
  size_t off,
  size_t tcnt,
  PIOL_File_ReadDirect* ifh,
  PIOL_File_WriteDirect* ofh)
{
    PIOL_File_Param* prm = PIOL_File_Param_new(NULL, tcnt);
    PIOL_File_ReadDirect_readParam(ifh, off, tcnt, prm);
    for (size_t i = 0; i < tcnt; i++) {
        PIOL_geom_t xval = PIOL_File_getPrm_double(i, PIOL_META_xSrc, prm);
        PIOL_geom_t yval = PIOL_File_getPrm_double(i, PIOL_META_ySrc, prm);
        PIOL_File_setPrm_double(i, PIOL_META_xSrc, yval, prm);
        PIOL_File_setPrm_double(i, PIOL_META_ySrc, xval, prm);
    }

    PIOL_File_WriteDirect_writeParam(ofh, off, tcnt, prm);
    PIOL_ExSeis_isErr(piol, "");
    PIOL_File_Param_delete(prm);
}

/*! Write the output header details.
 *  @param[in] piol The PIOL handle
 *  @param[in] ifh The input file handle
 *  @param[out] ofh The output file handle
 */
void writeHeader(
  PIOL_ExSeis* piol, PIOL_File_ReadDirect* ifh, PIOL_File_WriteDirect* ofh)
{
    PIOL_File_WriteDirect_writeText(ofh, PIOL_File_ReadDirect_readText(ifh));
    PIOL_File_WriteDirect_writeNs(ofh, PIOL_File_ReadDirect_readNs(ifh));
    PIOL_File_WriteDirect_writeNt(ofh, PIOL_File_ReadDirect_readNt(ifh));
    PIOL_File_WriteDirect_writeInc(ofh, PIOL_File_ReadDirect_readInc(ifh));
    PIOL_ExSeis_isErr(piol, "");
}

/*! Write the data from the input file to the output file
 *  @param[in] piol The PIOL handle
 *  @param[in] goff The global offset in number of traces.
 *  @param[in] lnt The lengthof each trace.
 *  @param[in] tcnt The number of traces.
 *  @param[in] ifh The input file handle
 *  @param[out] ofh The output file handle
 */
void writePayload(
  PIOL_ExSeis* piol,
  size_t goff,
  size_t lnt,
  size_t tcnt,
  PIOL_File_ReadDirect* ifh,
  PIOL_File_WriteDirect* ofh)
{
    size_t biggest = lnt;
    MPI_Allreduce(
      &lnt, &biggest, 1, MPI_UNSIGNED_LONG, MPI_MAX, MPI_COMM_WORLD);
    size_t extra =
      biggest / tcnt - lnt / tcnt + (biggest % tcnt > 0) - (lnt % tcnt > 0);

    for (size_t i = 0U; i < lnt; i += tcnt) {
        size_t rblock = (i + tcnt < lnt ? tcnt : lnt - i);
        readwriteParam(piol, goff + i, rblock, ifh, ofh);
    }

    for (size_t i = 0U; i < extra; i++) {
        readwriteParam(piol, goff, 0, ifh, ofh);
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

    PIOL_ExSeis* piol = PIOL_ExSeis_new(PIOL_VERBOSITY_NONE);
    PIOL_ExSeis_isErr(piol, "");

    PIOL_File_ReadDirect* ifh = PIOL_File_ReadDirect_new(piol, iname);
    PIOL_ExSeis_isErr(piol, "");

    size_t ns = PIOL_File_ReadDirect_readNs(ifh);
    size_t nt = PIOL_File_ReadDirect_readNt(ifh);
    // Write all header metadata
    PIOL_File_WriteDirect* ofh = PIOL_File_WriteDirect_new(piol, oname);
    PIOL_ExSeis_isErr(piol, "");

    writeHeader(piol, ifh, ofh);

    struct PIOL_Decomposed_range dec = PIOL_decompose_range(
      nt, PIOL_ExSeis_getNumRank(piol), PIOL_ExSeis_getRank(piol));
    size_t tcnt =
      memmax / max(PIOL_SEGY_utils_getDFSz(ns), PIOL_SEGY_utils_getMDSz());

    writePayload(piol, dec.global_offset, dec.local_size, tcnt, ifh, ofh);

    PIOL_ExSeis_isErr(piol, "");
    PIOL_File_WriteDirect_delete(ofh);
    PIOL_File_ReadDirect_delete(ifh);
    PIOL_ExSeis_delete(piol);

    free(iname);
    free(oname);

    return 0;
}
