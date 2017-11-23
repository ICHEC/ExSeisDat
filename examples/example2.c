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

    PIOL_ExSeisHandle piol = PIOL_ExSeis_new(PIOL_VERBOSITY_NONE);
    size_t rank = PIOL_ExSeis_getRank(piol);

    //Create a SEGY file object for input
    PIOL_File_ReadDirectHandle ifh = PIOL_File_ReadDirect_new(piol, iname);
    PIOL_ExSeis_isErr(piol, "Unable to create a SEGY file object for input.");

    //Create some local variables based on the input file
    size_t nt = PIOL_File_ReadDirect_readNt(ifh);
    size_t ns = PIOL_File_ReadDirect_readNs(ifh);

    Extent dec = decompose(nt, PIOL_ExSeis_getNumRank(piol), rank);
    size_t offset = dec.start;
    size_t lnt = dec.sz;

    //Alloc the required memory for the data we want.
    float * trace = malloc(lnt * PIOL_SEGSz_getDFSz(ns));
    PIOL_File_ParamHandle trhdr = PIOL_File_Param_new(NULL, lnt);

    //Create a SEGY file object for output
    PIOL_File_WriteDirectHandle ofh = PIOL_File_WriteDirect_new(piol, oname);
    PIOL_ExSeis_isErr(piol, "Unable to create a SEGY file object for output.");

    //Write the headers based on the input file.
    PIOL_File_WriteDirect_writeNs(ofh, nt);
    PIOL_File_WriteDirect_writeNt(ofh, ns);
    PIOL_File_WriteDirect_writeText(ofh, PIOL_File_ReadDirect_readText(ifh));
    PIOL_File_WriteDirect_writeInc(ofh,  PIOL_File_ReadDirect_readInc(ifh));

    //Read the traces from the input file and to the output
    PIOL_File_ReadDirect_readTrace(ifh, offset, lnt, trace, trhdr);
    PIOL_File_WriteDirect_writeTrace(ofh, offset, lnt, trace, trhdr);

    free(trace);
    PIOL_File_Param_delete(trhdr);

    //Close the file handles and close the piol
    PIOL_File_ReadDirect_delete(ifh);
    PIOL_File_WriteDirect_delete(ofh);
    PIOL_ExSeis_delete(piol);

    return 0;
}
