#include <stdio.h>
#include "cfileapi.h"
int main(void)
{
    ExSeisHandle piol = initMPIOL();
    if (!getRank(piol))
    {
        size_t num = getNumRank(piol);
        if (num == 1)
            printf("There is 1 process\n");
        else
            printf("There are %zu processes\n", num);
    }
    freePIOL(piol);
    return 0;
}
