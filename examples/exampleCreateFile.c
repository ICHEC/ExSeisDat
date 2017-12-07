#include "sglobal.h"
#include "cfileapi.h"
#include <assert.h>

void createFile(char name, size_t nt, size_t ns, size_t inc)
{
    //Initialise the PIOL by creating an ExSeis object
    PIOL_ExSeis* piol = PIOL_ExSeis_new(PIOL_VERBOSITY_NONE);

    //Create a SEGY file object
    PIOL_File_WriteDirect* fh = PIOL_File_WriteDirect_new(piol, name);

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
    PIOL_File_Param* prm = PIOL_File_Param_new(NULL, lnt);
    for (size_t j = 0; j < lnt; j++)
    {
        float k = offset+j;
        PIOL_File_setPrm_double(j, PIOL_META_xSrc, 1600.0 + k, prm);
        PIOL_File_setPrm_double(j, PIOL_META_ySrc, 2400.0 + k, prm);
        PIOL_File_setPrm_double(j, PIOL_META_xRcv, 100000.0 + k, prm);
        PIOL_File_setPrm_double(j, PIOL_META_yRcv, 3000000.0 + k, prm);
        PIOL_File_setPrm_double(j, PIOL_META_xCmp, 10000.0 + k, prm);
        PIOL_File_setPrm_double(j, PIOL_META_yCmp, 4000.0 + k, prm);
        PIOL_File_setPrm_llint(j, PIOL_META_il, 2400 + offset + j, prm);
        PIOL_File_setPrm_llint(j, PIOL_META_xl, 1600 + offset + j, prm);
        PIOL_File_setPrm_llint(j, PIOL_META_tn, offset + j, prm);
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
int main(void)
{
        //Set output file name, number of traces, number of samples per trace, and sampling rate
    char name = "CreateFileOutputC";
    size_t nt = 8000; ns = 4000;
    double inc = .01;

    createFile(name, nt, ns, inc);
    return 0;
}

