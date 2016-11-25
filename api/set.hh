/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date November 2016
 *   \brief The Set layer interface
*//*******************************************************************************************/
#ifndef APIPIOLSET_INCLUDE_GUARD
#define APIPIOLSET_INCLUDE_GUARD
#include "set/set.hh"
#include "fileops.hh"
namespace PIOL
{
class Set : public InternalSet
{
    public :
    Set(Piol piol_, std::string pattern, std::string outfix_ = "") : InternalSet(piol_, pattern, outfix_)
    {
    }
    void sort(File::SortType type);
};
}
#endif
