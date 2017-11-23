#include "sglobal.h"
#include "cfileapi.h"
#include <assert.h>
int main(int argc, char ** argv)
{
    //Initialise the PIOL by creating an ExSeis object
    PIOL_ExSeisHandle piol = PIOL_ExSeis_new(PIOL_VERBOSITY_NONE);

    char * opt = "o:";  //TODO: uses a GNU extension
    char * name = NULL;
    for (int c = getopt(argc, argv, opt); c != -1; c = getopt(argc, argv, opt))
        if (c == 'o')
            name = copyString(optarg);
        else
            fprintf(stderr, "One of the command line arguments is invalid\n");
    assert(name);

    //Create a SEGY file object
    PIOL_File_WriteDirectHandle fh = PIOL_File_WriteDirect_new(piol, name);

    //nt is the number of traces, ns the number of samples per trace
    size_t nt = 40000, ns = 1000;
    //inc is the increment step between traces (microseconds)
    double inc = 4.0;

    //Perform some decomposition (user decides how they will decompose)
    Extent dec = decompose(nt, PIOL_ExSeis_getNumRank(piol), PIOL_ExSeis_getRank(piol));

    //The offset for the local process
    size_t offset = dec.start;
    //The number of traces for the local process to handle
    size_t lnt = dec.sz;

    //Write some header parameters
    PIOL_File_WriteDirect_writeNs(fh, ns);
    PIOL_File_WriteDirect_writeNt(fh, nt);
    PIOL_File_WriteDirect_writeInc(fh, inc);
    PIOL_File_WriteDirect_writeText(fh, "Test file\n");

    //Set some trace parameters
    PIOL_File_ParamHandle prm = PIOL_File_Param_new(NULL, lnt);
    for (size_t j = 0; j < lnt; j++)
    {
        float k = offset+j;
        PIOL_File_setPrm_double(j, xSrc, 1600.0 + k, prm);
        PIOL_File_setPrm_double(j, ySrc, 2400.0 + k, prm);
        PIOL_File_setPrm_double(j, xRcv, 100000.0 + k, prm);
        PIOL_File_setPrm_double(j, yRcv, 3000000.0 + k, prm);
        PIOL_File_setPrm_double(j, xCmp, 10000.0 + k, prm);
        PIOL_File_setPrm_double(j, yCmp, 4000.0 + k, prm);
        PIOL_File_setPrm_llint(j, il, 2400 + offset + j, prm);
        PIOL_File_setPrm_llint(j, xl, 1600 + offset + j, prm);
        PIOL_File_setPrm_llint(j, tn, offset + j, prm);
    }

    //Set some traces
    float * trc = calloc(lnt*ns, sizeof(float));
    for (size_t j = 0; j < lnt*ns; j++)
    {
        trc[j] = (float)(offset*ns+j);
    }

    //Write the traces and trace parameters
    PIOL_File_WriteDirect_writeTrace(fh, offset, lnt, trc, prm);

    //Free the data
    PIOL_File_Param_delete(prm);
    free(trc);

    //Close the file handle and free the piol
    PIOL_File_WriteDirect_delete(fh);
    PIOL_ExSeis_delete(piol);

    return 0;
}

