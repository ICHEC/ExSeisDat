///
/// Example minmax.c    {#examples_minmax_c}
/// ================
///
/// @todo DOCUMENT ME - Finish documenting example.
///

#include "exseisdat/flow.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    // Initialise the PIOL by creating an ExSeisPIOL object
    piol_exseis* piol = piol_exseis_new(exseis_verbosity_none);

    char* opt  = "i:";  // TODO: uses a GNU extension
    char* name = NULL;
    for (int c = getopt(argc, argv, opt); c != -1;
         c     = getopt(argc, argv, opt)) {

        const size_t optarg_length = strlen(optarg) + 1;

        if (c == 'i') {
            free(name);
            name = malloc(optarg_length * sizeof(char));

            strncpy(name, optarg, optarg_length);
        }
        else {
            fprintf(stderr, "One of the command line arguments is invalid\n");
        }
    }
    assert(name);

    PIOL_Set* set = piol_set_new(piol, name);

    struct PIOL_CoordElem minmax[12];
    piol_set_get_min_max(set, exseis_meta_x_src, exseis_meta_y_src, &minmax[0]);
    piol_set_get_min_max(set, exseis_meta_x_rcv, exseis_meta_y_rcv, &minmax[4]);
    piol_set_get_min_max(set, exseis_meta_xCmp, exseis_meta_yCmp, &minmax[8]);

    if (!piol_exseis_get_rank(piol)) {
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

    piol_set_delete(set);
    free(name);
    piol_exseis_delete(piol);

    return 0;
}
