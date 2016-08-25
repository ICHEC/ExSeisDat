#include "ctest.h"
#include "cfileapi.h"
#include "sglobal.h"
#include <stddef.h>
#include <assert.h>

#warning TODO: Continue
typedef void (* modPrm)(TraceParam *);

void modifyTrPrm(modPrm func, size_t offset, size_t sz, TraceParam * prm)
{
    for (size_t i = 0; i < sz; i++)
        func(&prm[i]);
}

void readWriteTraceParam(ExSeisHandle piol, ExSeisFile ifh, ExSeisFile ofh, size_t off, size_t tcnt)
{
    TraceParam * trhdr = malloc(tcnt * sizeof(TraceParam));
    assert(trhdr);
    readTraceParam(ifh, off, tcnt, trhdr);
    writeTraceParam(ofh, off, tcnt, trhdr);
    isErr(piol);
    free(trhdr);
}

void readWriteTrace(ExSeisHandle piol, ExSeisFile ifh, ExSeisFile ofh, size_t off, size_t tcnt)
{
    size_t ns = readNs(ifh);
    float * trace = malloc(tcnt * getSEGYTraceLen(ns));
    assert(trace);

    readTrace(ifh, off, tcnt, trace);
    writeTrace(ofh, off, tcnt, trace);
    isErr(piol);
    free(trace);
}

void writePayload(ExSeisHandle piol, ExSeisFile ifh, ExSeisFile ofh, size_t goff, size_t lnt, size_t tcnt)
{
    size_t q = lnt / tcnt;
    size_t r = lnt % tcnt;

    printf("lnt %zu tcnt %zu q %zu r %zu\n", lnt, tcnt, q, r);
    for (size_t i = 0U; i < q; i++)
    {
        size_t off = goff + i * tcnt;
        readWriteTraceParam(piol, ifh, ofh, off, tcnt);
        readWriteTrace(piol, ifh, ofh, off, tcnt);
    }

    readWriteTraceParam(piol, ifh, ofh, goff + lnt-r, r);
    readWriteTrace(piol, ifh, ofh, goff + lnt-r, r);
    barrier(piol);
}

//template <bool ModifyParam, bool ModifyTrace>
int testReadWrite(ExSeisHandle piol, const char * iname, const char * oname, size_t memmax)
{
    ExSeisFile ifh = openReadFile(piol, iname);

    size_t ns = readNs(ifh);
    size_t nt = readNt(ifh);
    size_t rank = getRank(piol);
    //Write all header metadata
    if (!rank)
        printf("Open write mode file\n");
    ExSeisFile ofh = openWriteFile(piol, oname);

    if (!rank)
    {
        printf("%zu %zu %zu %zu\n", nt, (memmax * 1024U * 1024U), getSEGYTraceLen(ns), 4U*sizeof(TraceParam));
        printf("Write metadata to file %zu %zu %e\n", ns, nt, readInc(ifh));
    }

    writeText(ofh, readText(ifh));
    writeNs(ofh, ns);
    writeNt(ofh, nt);
    writeInc(ofh, readInc(ifh));
    isErr(piol);

    Extent dec = decompose(nt, getNumRank(piol), getRank(piol));

    size_t goff = dec.start;
    size_t lnt = dec.end;
    size_t tcnt = (memmax * 1024U * 1024U) / MAX(getSEGYTraceLen(ns), getSEGYParamSz());

    writePayload(piol, ifh, ofh, goff, lnt, tcnt);

    closeFile(ofh);
    closeFile(ifh);
    isErr(piol);
    return 0;
}
