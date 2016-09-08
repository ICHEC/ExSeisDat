#include "sglobal.h"
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include "cfileapi.h"

int main(int argc, char ** argv)
{
    char * opt = "i:o:";  //TODO: uses a GNU extension
    char * iname = NULL;
    char * oname = NULL;
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
    size_t rank = getRank(piol);

    //Create a SEGY file object for input
    ExSeisFile ifh = openReadFile(piol, iname);
    isErr(piol);

    //Create some local variables based on the input file
    size_t nt = readNt(ifh);
    size_t ns = readNs(ifh);

    Extent dec = decompose(nt, getNumRank(piol), rank);
    size_t offset = dec.start;
    size_t lnt = dec.end;

    //Alloc the required memory for the data we want.
    float * trace = malloc(lnt * getSEGYTraceLen(ns));
    TraceParam * trhdr = malloc(lnt * sizeof(TraceParam));

    //Create a SEGY file object for output
    ExSeisFile ofh = openWriteFile(piol, oname);
    isErr(piol);

    //Write the headers based on the input file.
    writeText(ofh, readText(ifh));
    writeNs(ofh, readNs(ifh));
    writeNt(ofh, readNt(ifh));
    writeInc(ofh, readInc(ifh));

    //Read the trace parameters from the input file and to the output
    readTraceParam(ifh, offset, lnt, trhdr);
    writeTraceParam(ofh, offset, lnt, trhdr);

    //Read the traces from the input file and to the output
    readTrace(ifh, offset, lnt, trace);
    writeTrace(ofh, offset, lnt, trace);

    free(trace);
    free(trhdr);

    //Close the file handles and close the piol
    closeFile(ifh);
    closeFile(ofh);
    closePIOL(piol);
    return 0;
}
