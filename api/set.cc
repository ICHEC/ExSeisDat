/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date November 2016
 *   \brief The Set layer interface
*//*******************************************************************************************/
#include "set.hh"

namespace PIOL {
void Set::sort(File::SortType type)
{
    sort(getComp(SortType type));
}
}
