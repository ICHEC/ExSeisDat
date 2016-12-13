#include "sglobal.h"
#include <unistd.h>
#include <stddef.h>
#include <assert.h>
#include "ctest.h"
#include "cfileapi.h"

int testManyFiles(ExSeisHandle piol, const char * name)
{
    //Don't go too crazy or systems won't like you.
    const size_t rnum = 10;
    const size_t fnum = 1000;
    ExSeisFile * file = calloc(fnum, sizeof(ExSeisFile));
    ExSeisFile ffile = openReadFile(piol, name);

    const char * msg = readText(ffile);
    size_t ln = strlen(msg);
    size_t ns = readNs(ffile);
    size_t nt = readNt(ffile);
    double inc = readInc(ffile);

    for (size_t i = 0; i < rnum; i++)
    {
        for (size_t j = 0; j < fnum; j++)
            file[j] = openReadFile(piol, name);

        for (size_t j = 0; j < fnum; j++)
        {
            CMP_STR(msg, readText(file[i]));
            CMP(ln, strlen(msg));
            CMP(ns, readNs(file[i]));
            CMP(nt, readNt(file[i]));
            CMP(inc, readInc(file[i]));
        }

        for (size_t j = 0; j < fnum; j++)
            closeFile(file[j]);
    }
    free(file);
    closeFile(ffile);

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

    ExSeisHandle piol = initMPIOL();
    isErr(piol);

    testManyFiles(piol, name);

    if (name != NULL)
        free(name);
    return 0;
}



