///
/// Example: sort.c    {#examples_sort_c}
/// ===============
///
/// @todo DOCUMENT ME - Finish documenting example.
///

#include "flow.h"

int main(void)
{
    PIOL_ExSeis* piol = PIOL_ExSeis_new(PIOL_VERBOSITY_NONE);
    PIOL_Set* set     = PIOL_Set_new(piol, "/ichec/work/exseisdat/*dat/10*/b*");
    PIOL_Set_output(set, "presort");
    PIOL_Set_sort(set, PIOL_SORTTYPE_OffLine);
    PIOL_Set_output(set, "postsort");
    PIOL_Set_delete(set);
    PIOL_ExSeis_delete(piol);
    return 0;
}
