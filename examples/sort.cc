/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date February 2017
 *   \brief Sort a file in 9 lines
*//*******************************************************************************************/
#include "flow.hh"
using namespace PIOL;
int main(void)
{
    ExSeis piol;
    Set set(piol.piol(), "/ichec/work/exseisdat/dat/10*/b*", "temp");
    set.sort(PIOL_SORTTYPE_OffLine);
    return 0;
}
