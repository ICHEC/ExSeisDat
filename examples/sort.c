#include "flow.h"
int main(void)
{
    PIOL_ExSeisHandle piol = PIOL_ExSeis_new();
    PIOL_SetHandle set = PIOL_Set_new(piol, "/ichec/work/exseisdat/dat/10*/b*");
    PIOL_Set_output(set, "presort");
    PIOL_Set_sort(set, OffLine);
    PIOL_Set_output(set, "postsort");
    PIOL_Set_delete(set);
    PIOL_ExSeis_delete(piol);
    return 0;
}
