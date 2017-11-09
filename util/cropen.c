#include "sglobal.h"
#include <unistd.h>
#include <stddef.h>
#include <assert.h>
#include "ctest.h"
#include "cfileapi.h"

int testManyFiles(PIOL_ExSeisHandle piol, const char * name)
{
    //Don't go too crazy or systems won't like you.
    const size_t rnum = 10;
    const size_t fnum = 1000;
    PIOL_File_ReadDirectHandle * files = calloc(
        fnum, sizeof(PIOL_File_ReadDirectHandle)
    );
    PIOL_File_ReadDirectHandle ffile = PIOL_File_ReadDirect_new(piol, name);

    const char * msg = PIOL_File_ReadDirect_readText(ffile);
    size_t ln  = strlen(msg);
    size_t ns  = PIOL_File_ReadDirect_readNs(ffile);
    size_t nt  = PIOL_File_ReadDirect_readNt(ffile);
    double inc = PIOL_File_ReadDirect_readInc(ffile);

    for (size_t i = 0; i < rnum; i++)
    {
        for (size_t j = 0; j < fnum; j++)
            files[j] = PIOL_File_ReadDirect_new(piol, name);

        for (size_t j = 0; j < fnum; j++)
        {
            CMP_STR(msg, PIOL_File_ReadDirect_readText(files[i]));
            CMP(ln, strlen(msg));
            CMP(ns,  PIOL_File_ReadDirect_readNs(files[i]));
            CMP(nt,  PIOL_File_ReadDirect_readNt(files[i]));
            CMP(inc, PIOL_File_ReadDirect_readInc(files[i]));
        }

        for (size_t j = 0; j < fnum; j++)
        {
            PIOL_File_ReadDirect_delete(files[j]);
        }
    }

    PIOL_File_ReadDirect_delete(ffile);
    free(files);

    return 0;
}

int main(int argc, char ** argv)
{
//  Flags:
// -i input file
// -o output file
// -m maximum memory
    char * opt = "i:";  //TODO: uses a GNU extension
    char * name = NULL;
    for (int c = getopt(argc, argv, opt); c != -1; c = getopt(argc, argv, opt))
        if (c == 'i')
            name = copyString(optarg);
        else
        {
            fprintf(stderr, "One of the command line arguments is invalid\n");
            return -1;
        }
    assert(name);

    PIOL_ExSeisHandle piol = PIOL_ExSeis_new();
    PIOL_ExSeis_isErr(piol);

    testManyFiles(piol, name);

    free(name);

    return 0;
}



