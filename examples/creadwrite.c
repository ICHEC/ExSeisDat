/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date Q4 2016
 *   \brief This file reads from an existing file, interprets it by the PIOL and writes out a
 *          new file. Since the input is interpreted by the PIOL it does not produce an identical
 *          copy.
*//*******************************************************************************************/
#include "sglobal.h"
#include <unistd.h>
#include <stddef.h>
#include <assert.h>
#include "ctest.h"
#include "cfileapi.h"

void readWriteFullTrace(ExSeisHandle piol, ExSeisRead ifh, ExSeisWrite ofh, size_t off, size_t tcnt, ModTrc ftrc, ModPrm fprm)
{
    CParam trhdr = initDefParam(tcnt);
    size_t ns = readNs(ifh);
    float * trace = malloc(tcnt * getSEGYTraceLen(ns));
    assert(trace);

    readFullTrace(ifh, off, tcnt, trace, trhdr);
    if (ftrc != NULL)
        for (size_t i = 0; i < tcnt; i++)
            ftrc(off, ns, &trace[i]);

    if (fprm != NULL)
        for (size_t i = 0; i < tcnt; i++)
            fprm(off, i, trhdr);

    writeFullTrace(ofh, off, tcnt, trace, trhdr);
    isErr(piol);

    free(trace);
    freeParam(trhdr);
}

void writePayload(ExSeisHandle piol, ExSeisRead ifh, ExSeisWrite ofh,
                  size_t goff, size_t lnt, size_t tcnt,
                  ModPrm fprm, ModTrc ftrc)
{
    size_t biggest = lnt;
    int err = MPI_Allreduce(&lnt, &biggest, 1, MPI_UNSIGNED_LONG, MPI_MAX, MPI_COMM_WORLD);
    size_t extra = biggest/tcnt - lnt/tcnt + (biggest % tcnt > 0) - (lnt % tcnt > 0);

    for (size_t i = 0U; i < lnt; i += tcnt)
        readWriteFullTrace(piol, ifh, ofh, goff+i, (i + tcnt < lnt ? tcnt : lnt - i), ftrc, fprm);

    for (size_t i = 0U; i < extra; i++)
        readWriteFullTrace(piol, ifh, ofh, goff, 0, ftrc, fprm);
}

int ReadWriteFile(ExSeisHandle piol, const char * iname, const char * oname, size_t memmax, ModPrm fprm, ModTrc ftrc)
{
    ExSeisRead ifh = openReadFile(piol, iname);
    isErr(piol);

    size_t ns = readNs(ifh);
    size_t nt = readNt(ifh);
    //Write all header metadata
    ExSeisWrite ofh = openWriteFile(piol, oname);
    isErr(piol);

    writeText(ofh, readText(ifh));
    writeNs(ofh, readNs(ifh));
    writeNt(ofh, readNt(ifh));
    writeInc(ofh, readInc(ifh));
    isErr(piol);

    Extent dec = decompose(nt, getNumRank(piol), getRank(piol));
    size_t tcnt = memmax / MAX(getSEGYTraceLen(ns), getSEGYParamSz());

    writePayload(piol, ifh, ofh, dec.start, dec.sz, tcnt, fprm, ftrc);

    isErr(piol);
    closeWriteFile(ofh);
    closeReadFile(ifh);
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

