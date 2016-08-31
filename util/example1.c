#include <stdlib.h>
#include "cfileapi.h"
int main(void)
{
    //Initialise the PIOL by creating an ExSeisPIOL object
    ExSeisHandle piol = initMPIOL();

    size_t rank = getRank(piol);
    size_t numRank = getNumRank(piol);

    //Create a SEGY file object
    ExSeisFile fh = openWriteFile(piol, "test.segy");

    //lnt is the number of traces and sets of trace parameters we will write per process
    size_t lnt = 40, ns = 300;
    double inc = 0.04;

    //Write some header parameters
    writeNs(fh, ns);
    writeNt(fh, lnt*numRank);
    writeInc(fh, inc);
    writeText(fh, "Test file\n");

    //Set and write some trace parameters
    TraceParam * prm = calloc(lnt, sizeof(TraceParam));
    for (size_t j = 0; j < lnt; j++)
    {
        float k = lnt*rank+j;
        prm[j].src.x = 1600.0 + k;
        prm[j].src.y = 2400.0 + k;
        prm[j].rcv.x = 100000.0 + k;
        prm[j].rcv.y = 3000000.0 + k;
        prm[j].cmp.x = 10000.0 + k;
        prm[j].cmp.y = 4000.0 + k;
        prm[j].line.il = 2400 + k;
        prm[j].line.xl = 1600 + k;
        prm[j].tn = lnt*rank+j;
    }
    writeTraceParam(fh, lnt*rank, lnt, prm);
    free(prm);

    //Set and write some traces
    float * trc = calloc(lnt*ns, sizeof(float));
    for (size_t j = 0; j < lnt*ns; j++)
        trc[j] = (float)(lnt*rank+j);
    writeTrace(fh, lnt*rank, lnt, trc);
    free(trc);

    //Close the file handle and close the piol
    closeFile(fh);
    closePIOL(piol);

    return 0;
}

