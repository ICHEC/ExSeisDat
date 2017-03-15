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
void readwriteParam(ExSeisHandle piol, size_t off, size_t tcnt, ExSeisRead ifh, ExSeisWrite ofh)
{
    CParam prm = initDefParam(tcnt);
    readParam(ifh, off, tcnt, prm);
    for (size_t i = 0; i < tcnt; i++)
    {
        geom_t xval = getFloatPrm(i, xSrc, prm);
        geom_t yval = getFloatPrm(i, ySrc, prm);
        setFloatPrm(i, xSrc, yval, prm);
        setFloatPrm(i, ySrc, xval, prm);
    }

    writeParam(ofh, off, tcnt, prm);
    isErr(piol);
    freeParam(prm);
}

/*! Write the output header details.
 *  \param[in] piol The PIOL handle
 *  \param[in] ifh The input file handle
 *  \param[out] ofh The output file handle
 */
void writeHeader(ExSeisHandle piol, ExSeisRead ifh, ExSeisWrite ofh)
{
    writeText(ofh, readText(ifh));
    writeNs(ofh, readNs(ifh));
    writeNt(ofh, readNt(ifh));
    writeInc(ofh, readInc(ifh));
    isErr(piol);
}

/*! Write the data from the input file to the output file
 *  \param[in] piol The PIOL handle
 *  \param[in] goff The global offset in number of traces.
 *  \param[in] lnt The lengthof each trace.
 *  \param[in] tcnt The number of traces.
 *  \param[in] ifh The input file handle
 *  \param[out] ofh The output file handle
 */
void writePayload(ExSeisHandle piol, size_t goff, size_t lnt, size_t tcnt,
                  ExSeisRead ifh, ExSeisWrite ofh)
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
        readwriteParam(piol, goff, 0, ifh, ofh);
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

    ExSeisHandle piol = initMPIOL();
    isErr(piol);

    ExSeisRead ifh = openReadFile(piol, iname);
    isErr(piol);

    size_t ns = readNs(ifh);
    size_t nt = readNt(ifh);
    //Write all header metadata
    ExSeisWrite ofh = openWriteFile(piol, oname);
    isErr(piol);

    writeHeader(piol, ifh, ofh);

    Extent dec = decompose(nt, getNumRank(piol), getRank(piol));
    size_t tcnt = memmax / MAX(getSEGYTraceLen(ns), getSEGYParamSz());

    writePayload(piol, dec.start, dec.sz, tcnt, ifh, ofh);

    isErr(piol);
    closeWriteFile(ofh);
    closeReadFile(ifh);
    freePIOL(piol);

    if (iname != NULL)
        free(iname);
    if (oname != NULL)
        free(oname);
    return 0;
}

