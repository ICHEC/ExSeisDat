#include <stdio.h>
#include "cfileapi.h"
int main(void)
{
    PIOL_ExSeisHandle piol = PIOL_ExSeis_new(PIOL_VERBOSITY_NONE);
    if (!PIOL_ExSeis_getRank(piol))
    {
        size_t num = PIOL_ExSeis_getNumRank(piol);
        if (num == 1)
            printf("There is 1 process\n");
        else
            printf("There are %zu processes\n", num);
    }
    PIOL_ExSeis_delete(piol);
    return 0;
}
