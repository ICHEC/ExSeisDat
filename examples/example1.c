#include "sglobal.h"
#include "cfileapi.h"
#include <assert.h>
int main(int argc, char ** argv)
{
    //Initialise the PIOL by creating an ExSeisPIOL object
    ExSeisHandle piol = initMPIOL();

    char * opt = "o:";  //TODO: uses a GNU extension
    char * name = NULL;
    for (int c = getopt(argc, argv, opt); c != -1; c = getopt(argc, argv, opt))
        if (c == 'o')
            name = copyString(optarg);
        else
            fprintf(stderr, "One of the command line arguments is invalid\n");
    assert(name);

    //Create a SEGY file object
    ExSeisFile fh = openWriteFile(piol, name);

    //nt is the number of traces, ns the number of samples per trace
    size_t nt = 400000, ns = 1000;
    //inc is the increment step between traces (microseconds)
    double inc = 4.0;

    //Perform some decomposition (user decides how they will decompose)
    Extent dec = decompose(nt, getNumRank(piol), getRank(piol));

    //The offset for the local process
    size_t offset = dec.start;
    //The number of traces for the local process to handle
    size_t lnt = dec.sz;

    //Write some header parameters
    writeNs(fh, ns);
    writeNt(fh, nt);
    writeInc(fh, inc);
    writeText(fh, "Test file\n");

    //Set some trace parameters
    CParam prm = initDefParam(lnt);
    for (size_t j = 0; j < lnt; j++)
    {
        float k = offset+j;
        setFloatPrm(j, xSrc, 1600.0 + k, prm);
        setFloatPrm(j, ySrc, 2400.0 + k, prm);
        setFloatPrm(j, xRcv, 100000.0 + k, prm);
        setFloatPrm(j, yRcv, 3000000.0 + k, prm);
        setFloatPrm(j, xCmp, 10000.0 + k, prm);
        setFloatPrm(j, yCmp, 4000.0 + k, prm);
        setLongPrm(j, il, 2400 + offset + j, prm);
        setLongPrm(j, xl, 1600 + offset + j, prm);
        setLongPrm(j, tn, offset + j, prm);
    }

    //Set some traces
    float * trc = calloc(lnt*ns, sizeof(float));
    for (size_t j = 0; j < lnt*ns; j++)
        trc[j] = (float)(offset*ns+j);

    //Write the traces and trace parameters
    writeFullTrace(fh, offset, lnt, trc, prm);

    //Free the data
    freeParam(prm);
    free(trc);

    //Close the file handle and free the piol
    closeFile(fh);
    freePIOL(piol);

    return 0;
}

