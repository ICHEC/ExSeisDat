#include "sglobal.h"
#include "set.h"
#include <assert.h>
int main(int argc, char ** argv)
{
    //Initialise the PIOL by creating an ExSeisPIOL object
    ExSeisHandle piol = initMPIOL();

    char * opt = "i:";  //TODO: uses a GNU extension
    char * name = NULL;
    for (int c = getopt(argc, argv, opt); c != -1; c = getopt(argc, argv, opt))
        if (c == 'i')
            name = copyString(optarg);
        else
            fprintf(stderr, "One of the command line arguments is invalid\n");
    assert(name);

    ExSeisSet set = initSet(piol, name);

    CoordElem minmax[12];
    getMinMaxSet(set, xSrc, ySrc, &minmax[0]);
    getMinMaxSet(set, xRcv, yRcv, &minmax[4]);
    getMinMaxSet(set, xCmp, yCmp, &minmax[8]);

    if (!getRank(piol))
    {
        printf("x Src %e (%zu) -> %e (%zu)\n", minmax[0].val, minmax[0].num, minmax[1].val, minmax[1].num);
        printf("y Src %e (%zu) -> %e (%zu)\n", minmax[2].val, minmax[2].num, minmax[3].val, minmax[3].num);
        printf("x Rcv %e (%zu) -> %e (%zu)\n", minmax[4].val, minmax[4].num, minmax[5].val, minmax[5].num);
        printf("y Rcv %e (%zu) -> %e (%zu)\n", minmax[6].val, minmax[6].num, minmax[7].val, minmax[7].num);
        printf("x Cmp %e (%zu) -> %e (%zu)\n", minmax[8].val, minmax[8].num, minmax[9].val, minmax[9].num);
        printf("y Cmp %e (%zu) -> %e (%zu)\n", minmax[10].val, minmax[10].num, minmax[11].val, minmax[11].num);
    }
    return 0;
}
