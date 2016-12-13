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
                fprintf(stderr, "Invalid command line arguments\n");
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
    size_t lnt = dec.sz;

    //Alloc the required memory for the data we want.
    float * trace = malloc(lnt * getSEGYTraceLen(ns));
    CParam trhdr = initDefParam(lnt);

    //Create a SEGY file object for output
    ExSeisFile ofh = openWriteFile(piol, oname);
    isErr(piol);

    //Write the headers based on the input file.
    writeNs(ofh, nt);
    writeNt(ofh, ns);
    writeText(ofh, readText(ifh));
    writeInc(ofh, readInc(ifh));

    //Read the traces from the input file and to the output
    readFullTrace(ifh, offset, lnt, trace, trhdr);
    writeFullTrace(ofh, offset, lnt, trace, trhdr);

    free(trace);
    freeParam(trhdr);

    //Close the file handles and close the piol
    closeFile(ifh);
    closeFile(ofh);
    freePIOL(piol);
    return 0;
}
