///
/// Example: sort.c    {#examples_sort_c}
/// ===============
///
/// @todo DOCUMENT ME - Finish documenting example.
///

#include "exseisdat/flow.h"

int main()
{
    piol_exseis* piol = piol_exseis_new(exseis_verbosity_none);
    PIOL_Set* set     = piol_set_new(piol, "/ichec/work/exseisdat/*dat/10*/b*");
    piol_set_output(set, "presort");
    piol_set_sort(set, exseis_sorttype_OffLine);
    piol_set_output(set, "postsort");
    piol_set_delete(set);
    piol_exseis_delete(piol);
    return 0;
}
