#include "cfileapi.h"
#include <stdlib.h>

int main(void)
{
    //Initialise the PIOL by creating an ExSeisPIOL object
    ExSeisHandle piol = initMPIOL();
    size_t rank = getRank(piol);

    //Create a SEGY file object for input
    ExSeisFile ifh = openReadFile(piol, "test.segy");
    isErr(piol);

    //Create some local variables based on the input file
    size_t nt = readNt(ifh);
    size_t ns = readNs(ifh);

    //lnt is a local subset of the number of traces
    size_t lnt = nt / getNumRank(piol);

    //Alloc the required memory for the data we want.
    float * trace = malloc(lnt * getSEGYTraceLen(ns));
    TraceParam * trhdr = malloc(lnt * sizeof(TraceParam));

    //Create a SEGY file object for output
    ExSeisFile ofh = openWriteFile(piol, "test1.segy");
    isErr(piol);

    //Write the headers based on the input file.
    writeText(ofh, readText(ifh));
    writeNs(ofh, readNs(ifh));
    writeNt(ofh, readNt(ifh));
    writeInc(ofh, readInc(ifh));

    //Read the trace parameters from the input file and to the output
    readTraceParam(ifh, lnt * rank, lnt, trhdr);
    writeTraceParam(ofh, lnt * rank, lnt, trhdr);

    //Read the traces from the input file and to the output
    readTrace(ifh, lnt * rank, lnt, trace);
    writeTrace(ofh, lnt * rank, lnt, trace);

    free(trace);
    free(trhdr);

    //Close the file handles and close the piol
    closeFile(ifh);
    closeFile(ofh);
    closePIOL(piol);
    return 0;
}
