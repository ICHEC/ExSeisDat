#include "sglobal.h"
#include <unistd.h>
#include <stddef.h>
#include <assert.h>
#include "ctest.h"
#include "cfileapi.h"

void readWriteParam(ExSeisHandle piol, ExSeisFile ifh, ExSeisFile ofh, size_t off, size_t tcnt, ModPrm fprm)
{
    CParam trhdr = initDefParam(tcnt);
    readParam(ifh, off, tcnt, trhdr);

    if (fprm != NULL)
        for (size_t i = 0; i < tcnt; i++)
            fprm(off, i, trhdr);

    writeParam(ofh, off, tcnt, trhdr);
    isErr(piol);
    freeParam(trhdr);
}

void readWriteTrace(ExSeisHandle piol, ExSeisFile ifh, ExSeisFile ofh, size_t off, size_t tcnt, ModTrc ftrc)
{
    size_t ns = readNs(ifh);
    float * trace = malloc(tcnt * getSEGYTraceLen(ns));
    assert(trace);

    readTrace(ifh, off, tcnt, trace);
    if (ftrc != NULL)
        for (size_t i = 0; i < tcnt; i++)
            ftrc(off, ns, &trace[i]);
    writeTrace(ofh, off, tcnt, trace);

    isErr(piol);
    free(trace);
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
                  size_t goff, size_t lnt, size_t tcnt,
                  ModPrm fprm, ModTrc ftrc)
{
    size_t biggest = lnt;
    int err = MPI_Allreduce(&lnt, &biggest, 1, MPI_UNSIGNED_LONG, MPI_MAX, MPI_COMM_WORLD);
    size_t extra = biggest/tcnt - lnt/tcnt + (biggest % tcnt > 0) - (lnt % tcnt > 0);

    for (size_t i = 0U; i < lnt; i += tcnt)
    {
        size_t rblock = (i + tcnt < lnt ? tcnt : lnt - i);
        readWriteParam(piol, ifh, ofh, goff+i, rblock, fprm);
        readWriteTrace(piol, ifh, ofh, goff+i, rblock, ftrc);
    }

    for (size_t i = 0U; i < extra; i++)
    {
        readWriteParam(piol, ifh, ofh, goff, 0, fprm);
        readWriteTrace(piol, ifh, ofh, goff, 0, ftrc);
    }

}

int ReadWriteFile(ExSeisHandle piol, const char * iname, const char * oname, size_t memmax, ModPrm fprm, ModTrc ftrc)
{
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

    writePayload(piol, ifh, ofh, dec.start, dec.sz, tcnt, fprm, ftrc);

    isErr(piol);
    closeFile(ofh);
    closeFile(ifh);
    return 0;
}

void SourceX1600Y2400(size_t offset, size_t i, CParam prm)
{
    setFloatPrm(i, xSrc, offset + 1600.0, prm);
    setFloatPrm(i, ySrc, offset + 2400.0, prm);
}

void TraceLinearInc(size_t offset, size_t ns, float * trc)
{
    for (size_t i = 0; i < ns; i++)
        trc[i] = 2.0*i + offset;
}


int main(int argc, char ** argv)
{
//  Flags:
// -i input file
// -o output file
// -m maximum memory
// -p modify trace paramaters
// -t modify trace values
    char * opt = "i:o:m:pt";  //TODO: uses a GNU extension
    char * iname = NULL;
    char * oname = NULL;
    size_t memmax = 2U*1024U * 1024U * 1024U;   //bytes
    ModPrm modPrm = NULL;
    ModTrc modTrc = false;
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
            case 'm' :
            if (sscanf(optarg, "%zu", &memmax) != 1)
            {
                fprintf(stderr, "Incorrect arguments to memmax option\n");
                return -1;
            }
            break;
            case 'p' :
                printf("The trace parameters will be modified\n");
                modPrm = SourceX1600Y2400;
            break;
            case 't' :
                printf("The traces will be modified\n");
                modTrc = TraceLinearInc;
            break;
            default :
                fprintf(stderr, "One of the command line arguments is invalid\n");
            break;
        }
    assert(iname && oname);

    ExSeisHandle piol = initMPIOL();
    isErr(piol);

    ReadWriteFile(piol, iname, oname, memmax, modPrm, modTrc);

    freePIOL(piol);

    if (iname != NULL)
        free(iname);
    if (oname != NULL)
        free(oname);
    return 0;
}

