#include "flow.h"
int main(void)
{
    ExSeisHandle piol = initMPIOL();
    ExSeisSet set = initSet(piol, "/ichec/work/exseisdat/dat/10*/b*");
    outputSet(set, "presort");
    sortSet(set, OffLine);
    outputSet(set, "postsort");
    freeSet(set);
    freePIOL(piol);
    return 0;
}
