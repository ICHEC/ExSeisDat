#include "sglobal.h"
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include "cfileapi.h"
int main(int argc, char ** argv)
{
    char * opt = "o:";  //TODO: uses a GNU extension
    char * name = NULL;
    for (int c = getopt(argc, argv, opt); c != -1; c = getopt(argc, argv, opt))
        if (c == 'o')
            name = copyString(optarg);
        else
        {
            fprintf(stderr, "One of the command line arguments is invalid\n");
            return -1;
        }
    assert(name);

    //Initialise the PIOL by creating an ExSeisPIOL object
    ExSeisHandle piol = initMPIOL();

    //Create a SEGY file object
    ExSeisFile fh = openWriteFile(piol, name);

    //lnt is the number of traces and sets of trace parameters we will write per process
    size_t nt = 40000, ns = 300;
    double inc = 0.04;

    Extent dec = decompose(nt, getNumRank(piol), getRank(piol));
    size_t offset = dec.start;
    size_t lnt = dec.end;

    //Write some header parameters
    writeNs(fh, ns);
    writeNt(fh, nt);
    writeInc(fh, inc);
    writeText(fh, "Test file\n");

    //Set and write some trace parameters
    Param prm = newDefParam(lnt);
    for (size_t j = 0; j < lnt; j++)
    {
        float k = offset+j;
        setFloatPrm(j, xSrc, 1600.0 + k, prm);
        setFloatPrm(j, ySrc, 2400.0 + k, prm);
        setFloatPrm(j, xRcv, 100000.0 + k, prm);
        setFloatPrm(j, yRcv, 3000000.0 + k, prm);
        setFloatPrm(j, xCmp, 10000.0 + k, prm);
        setFloatPrm(j, yCmp, 4000.0 + k, prm);
        setLongPrm(j, il, 2400.0 + k, prm);
        setLongPrm(j, xl, 1600.0 + k, prm);
        setLongPrm(j, tn, offset + j, prm);
    }
    writeTraceParam(fh, offset, lnt, prm);
    free(prm);

    //Set and write some traces
    float * trc = calloc(lnt*ns, sizeof(float));
    for (size_t j = 0; j < lnt*ns; j++)
        trc[j] = (float)(offset*ns+j);
    writeTrace(fh, offset, lnt, trc);
    free(trc);

    //Close the file handle and close the piol
    closeFile(fh);
    freePIOL(piol);

    return 0;
}

