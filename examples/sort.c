#include "set.h"
int main(void)
{
    ExSeisHandle piol = initMPIOL();
    ExSeisSet set = initSet(piol, "/ichec/work/exseisdat/dat/10*/b*");
    sortSet(set, SrcRcv);
    outputSet(set, "ctemp");
    freeSet(set);
    freePIOL(piol);
    return 0;
}
