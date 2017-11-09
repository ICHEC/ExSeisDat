#include "sglobal.h"
#include <unistd.h>
#include <stddef.h>
#include <assert.h>
#include "ctest.h"
#include "cfileapi.h"

/*! Read nt parameters from the input file ifh and write it to
 *  the output file ofh. Parameters are read and written from
 *  the offset off.
 *  \param[in] piol The PIOL handle
 *  \param[in] off The offset in number of traces.
 *  \param[in] tcnt The number of traces.
 *  \param[in] ifh The input file handle
 *  \param[out] ofh The output file handle
 */
void readwriteParam(PIOL_ExSeisHandle piol, size_t off, size_t tcnt, PIOL_File_ReadDirectHandle ifh, PIOL_File_WriteDirectHandle ofh)
{
    PIOL_File_ParamHandle prm = PIOL_File_Param_new(NULL, tcnt);
    PIOL_File_ReadDirect_readParam(ifh, off, tcnt, prm);
    for (size_t i = 0; i < tcnt; i++)
    {
        geom_t xval = PIOL_File_getPrm_double(i, xSrc, prm);
        geom_t yval = PIOL_File_getPrm_double(i, ySrc, prm);
        PIOL_File_setPrm_double(i, xSrc, yval, prm);
        PIOL_File_setPrm_double(i, ySrc, xval, prm);
    }

    PIOL_File_WriteDirect_writeParam(ofh, off, tcnt, prm);
    PIOL_ExSeis_isErr(piol);
    PIOL_File_Param_delete(prm);
}

/*! Write the output header details.
 *  \param[in] piol The PIOL handle
 *  \param[in] ifh The input file handle
 *  \param[out] ofh The output file handle
 */
void writeHeader(PIOL_ExSeisHandle piol, PIOL_File_ReadDirectHandle ifh, PIOL_File_WriteDirectHandle ofh)
{
    PIOL_File_WriteDirect_writeText(ofh, PIOL_File_ReadDirect_readText(ifh));
    PIOL_File_WriteDirect_writeNs(ofh,  PIOL_File_ReadDirect_readNs(ifh));
    PIOL_File_WriteDirect_writeNt(ofh,  PIOL_File_ReadDirect_readNt(ifh));
    PIOL_File_WriteDirect_writeInc(ofh, PIOL_File_ReadDirect_readInc(ifh));
    PIOL_ExSeis_isErr(piol);
}

/*! Write the data from the input file to the output file
 *  \param[in] piol The PIOL handle
 *  \param[in] goff The global offset in number of traces.
 *  \param[in] lnt The lengthof each trace.
 *  \param[in] tcnt The number of traces.
 *  \param[in] ifh The input file handle
 *  \param[out] ofh The output file handle
 */
void writePayload(PIOL_ExSeisHandle piol, size_t goff, size_t lnt, size_t tcnt,
                  PIOL_File_ReadDirectHandle ifh, PIOL_File_WriteDirectHandle ofh)
{
    size_t biggest = lnt;
    int err = MPI_Allreduce(&lnt, &biggest, 1, MPI_UNSIGNED_LONG, MPI_MAX, MPI_COMM_WORLD);
    size_t extra = biggest/tcnt - lnt/tcnt + (biggest % tcnt > 0) - (lnt % tcnt > 0);

    for (size_t i = 0U; i < lnt; i += tcnt)
    {
        size_t rblock = (i + tcnt < lnt ? tcnt : lnt - i);
        readwriteParam(piol, goff+i, rblock, ifh, ofh);
    }

    for (size_t i = 0U; i < extra; i++)
    {
        readwriteParam(piol, goff, 0, ifh, ofh);
    }
}

int main(int argc, char ** argv)
{
//  Flags:
// -i input file
// -o output file
// -m maximum memory
    char * opt = "i:o:";  //TODO: uses a GNU extension
    char * iname = NULL;
    char * oname = NULL;
    size_t memmax = 2U*1024U * 1024U * 1024U;   //bytes
    for (int c = getopt(argc, argv, opt); c != -1; c = getopt(argc, argv, opt))
        switch (c)
        {
            case 'i' :
                //TODO: POSIX is vague about the lifetime of optarg. Next function may be unnecessary
                iname = copyString(optarg);
            break;
            case 'o' :
                oname = copyString(optarg);
            break;
            default :
                fprintf(stderr, "One of the command line arguments is invalid\n");
            break;
        }
    assert(iname && oname);

    PIOL_ExSeisHandle piol = PIOL_ExSeis_new();
    PIOL_ExSeis_isErr(piol);

    PIOL_File_ReadDirectHandle ifh = PIOL_File_ReadDirect_new(piol, iname);
    PIOL_ExSeis_isErr(piol);

    size_t ns = PIOL_File_ReadDirect_readNs(ifh);
    size_t nt = PIOL_File_ReadDirect_readNt(ifh);
    //Write all header metadata
    PIOL_File_WriteDirectHandle ofh = PIOL_File_WriteDirect_new(piol, oname);
    PIOL_ExSeis_isErr(piol);

    writeHeader(piol, ifh, ofh);

    Extent dec = decompose(nt, PIOL_ExSeis_getNumRank(piol),
                           PIOL_ExSeis_getRank(piol));
    size_t tcnt = memmax / MAX(PIOL_SEGSz_getDFSz(ns), PIOL_SEGSz_getMDSz());

    writePayload(piol, dec.start, dec.sz, tcnt, ifh, ofh);

    PIOL_ExSeis_isErr(piol);
    PIOL_File_WriteDirect_delete(ofh);
    PIOL_File_ReadDirect_delete(ifh);
    PIOL_ExSeis_delete(piol);

    free(iname);
    free(oname);

    return 0;
}

