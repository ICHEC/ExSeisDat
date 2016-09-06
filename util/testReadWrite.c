#include "sglobal.h"
#include <unistd.h>
#include <stddef.h>
#include <assert.h>
#include "ctest.h"
#include "cfileapi.h"

void readWriteTraceParam(ExSeisHandle piol, ExSeisFile ifh, ExSeisFile ofh, size_t off, size_t tcnt, ModPrm fprm)
{
    TraceParam * trhdr = malloc(tcnt * sizeof(TraceParam));
    assert(trhdr);
    readTraceParam(ifh, off, tcnt, trhdr);

    if (fprm != NULL)
        for (size_t i = 0; i < tcnt; i++)
            fprm(off, &trhdr[i]);

    writeTraceParam(ofh, off, tcnt, trhdr);
    isErr(piol);
    free(trhdr);
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
    if (lnt == 0)   //No work to do
        return;

    size_t q = lnt / tcnt;
    size_t r = lnt % tcnt;

//    printf("rank %zu goff %zu lnt %zu tcnt %zu q %zu r %zu\n", getRank(piol), goff, lnt, tcnt, q, r);
    for (size_t i = 0U; i < q; i++)
    {
        size_t off = goff + i * tcnt;
        readWriteTraceParam(piol, ifh, ofh, off, tcnt, fprm);
        readWriteTrace(piol, ifh, ofh, off, tcnt, ftrc);
    }

    readWriteTraceParam(piol, ifh, ofh, goff + lnt-r, r, fprm);
    readWriteTrace(piol, ifh, ofh, goff + lnt-r, r, ftrc);
}

int ReadWriteFile(ExSeisHandle piol, const char * iname, const char * oname, size_t memmax, ModPrm fprm, ModTrc ftrc)
{
    ExSeisFile ifh = openReadFile(piol, iname);
    isErr(piol);

    size_t ns = readNs(ifh);
    size_t nt = readNt(ifh);
    size_t rank = getRank(piol);
    //Write all header metadata
    if (!rank)
        printf("Open write mode file\n");
    ExSeisFile ofh = openWriteFile(piol, oname);
    isErr(piol);

    if (!rank)
    {
        printf("%zu %zu %zu %zu\n", nt, memmax, getSEGYTraceLen(ns), 4U*sizeof(TraceParam));
        printf("Write metadata to file %zu %zu %e\n", ns, nt, readInc(ifh));
    }
    writeHeader(piol, ifh, ofh);

    Extent dec = decompose(nt, getNumRank(piol), getRank(piol));
    size_t tcnt = memmax / MAX(getSEGYTraceLen(ns), getSEGYParamSz());

    writePayload(piol, ifh, ofh, dec.start, dec.end, tcnt, fprm, ftrc);

    isErr(piol);
    closeFile(ofh);
    closeFile(ifh);
    return 0;
}

void SourceX1600Y2400(size_t offset, TraceParam * prm)
{
    prm->src.x = offset + 1600.0;
    prm->src.y = offset + 2400.0;
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

    ExSeisHandle piol = initPIOL(0, NULL);
    isErr(piol);

    ReadWriteFile(piol, iname, oname, memmax, modPrm, modTrc);

    closePIOL(piol);

    if (iname != NULL)
        free(iname);
    if (oname != NULL)
        free(oname);
    return 0;
}

