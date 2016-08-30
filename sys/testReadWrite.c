#include "ctest.h"
#include "cfileapi.h"
#include "sglobal.h"
#include <stddef.h>
#include <assert.h>

#warning TODO: Continue
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

int testReadWrite(ExSeisHandle piol, const char * iname, const char * oname, size_t memmax, ModPrm fprm, ModTrc ftrc)
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
        printf("%zu %zu %zu %zu\n", nt, (memmax * 1024U * 1024U), getSEGYTraceLen(ns), 4U*sizeof(TraceParam));
        printf("Write metadata to file %zu %zu %e\n", ns, nt, readInc(ifh));
    }
    writeHeader(piol, ifh, ofh);

    Extent dec = decompose(nt, getNumRank(piol), getRank(piol));
    size_t tcnt = (memmax * 1024U * 1024U) / MAX(getSEGYTraceLen(ns), getSEGYParamSz());

    writePayload(piol, ifh, ofh, dec.start, dec.end, tcnt, NULL, NULL);

    closeFile(ofh);
    closeFile(ifh);
    isErr(piol);
    return 0;
}
