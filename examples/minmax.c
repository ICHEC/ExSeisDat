#include "flow.h"
#include "sglobal.h"

#include <assert.h>

int main(int argc, char** argv)
{
    // Initialise the PIOL by creating an ExSeisPIOL object
    PIOL_ExSeis* piol = PIOL_ExSeis_new(PIOL_VERBOSITY_NONE);

    char* opt  = "i:";  // TODO: uses a GNU extension
    char* name = NULL;
    for (int c = getopt(argc, argv, opt); c != -1; c = getopt(argc, argv, opt))
        if (c == 'i')
            name = copyString(optarg);
        else
            fprintf(stderr, "One of the command line arguments is invalid\n");
    assert(name);

    PIOL_Set* set = PIOL_Set_new(piol, name);

    struct PIOL_CoordElem minmax[12];
    PIOL_Set_getMinMax(set, PIOL_META_xSrc, PIOL_META_ySrc, &minmax[0]);
    PIOL_Set_getMinMax(set, PIOL_META_xRcv, PIOL_META_yRcv, &minmax[4]);
    PIOL_Set_getMinMax(set, PIOL_META_xCmp, PIOL_META_yCmp, &minmax[8]);

    if (!PIOL_ExSeis_getRank(piol)) {
        printf(
          "x Src %e (%zu) -> %e (%zu)\n", minmax[0].val, minmax[0].num,
          minmax[1].val, minmax[1].num);
        printf(
          "y Src %e (%zu) -> %e (%zu)\n", minmax[2].val, minmax[2].num,
          minmax[3].val, minmax[3].num);
        printf(
          "x Rcv %e (%zu) -> %e (%zu)\n", minmax[4].val, minmax[4].num,
          minmax[5].val, minmax[5].num);
        printf(
          "y Rcv %e (%zu) -> %e (%zu)\n", minmax[6].val, minmax[6].num,
          minmax[7].val, minmax[7].num);
        printf(
          "x Cmp %e (%zu) -> %e (%zu)\n", minmax[8].val, minmax[8].num,
          minmax[9].val, minmax[9].num);
        printf(
          "y Cmp %e (%zu) -> %e (%zu)\n", minmax[10].val, minmax[10].num,
          minmax[11].val, minmax[11].num);
    }
    return 0;
}
