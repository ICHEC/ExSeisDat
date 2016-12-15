#include "sglobal.h"
#include <unistd.h>
#include <stddef.h>
#include <assert.h>
#include "ctest.h"
#include "cfileapi.h"

void readwriteParam(ExSeisHandle piol, ExSeisFile ifh, ExSeisFile ofh, size_t off, size_t tcnt)
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

void writeHeader(ExSeisHandle piol, ExSeisFile ifh, ExSeisFile ofh)
{
    writeText(ofh, readText(ifh));
    writeNs(ofh, readNs(ifh));
    writeNt(ofh, readNt(ifh));
    writeInc(ofh, readInc(ifh));
    isErr(piol);
}

void writePayload(ExSeisHandle piol, ExSeisFile ifh, ExSeisFile ofh,
                  size_t goff, size_t lnt, size_t tcnt)
{
    size_t biggest = lnt;
    int err = MPI_Allreduce(&lnt, &biggest, 1, MPI_UNSIGNED_LONG, MPI_MAX, MPI_COMM_WORLD);
    size_t extra = biggest/tcnt - lnt/tcnt + (biggest % tcnt > 0) - (lnt % tcnt > 0);

    for (size_t i = 0U; i < lnt; i += tcnt)
    {
        size_t rblock = (i + tcnt < lnt ? tcnt : lnt - i);
        readwriteParam(piol, ifh, ofh, goff+i, rblock);
    }

    for (size_t i = 0U; i < extra; i++)
        readwriteParam(piol, ifh, ofh, goff, 0);
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

    ExSeisFile ifh = openReadFile(piol, iname);
    isErr(piol);

    size_t ns = readNs(ifh);
    size_t nt = readNt(ifh);
    //Write all header metadata
    ExSeisFile ofh = openWriteFile(piol, oname);
    isErr(piol);

    writeHeader(piol, ifh, ofh);

    Extent dec = decompose(nt, getNumRank(piol), getRank(piol));
    size_t tcnt = memmax / MAX(getSEGYTraceLen(ns), getSEGYParamSz());

    writePayload(piol, ifh, ofh, dec.start, dec.sz, tcnt);

    isErr(piol);
    closeFile(ofh);
    closeFile(ifh);
    freePIOL(piol);

    if (iname != NULL)
        free(iname);
    if (oname != NULL)
        free(oname);
    return 0;
}
