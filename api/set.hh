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
extern std::shared_ptr<File::Rule> getMaxRules(void);

class Set : public InternalSet
{
    public :
//    Set(Piol piol_, std::string pattern, std::string outfix_ = "") : InternalSet(piol_, pattern, outfix_)
    Set(Piol piol_, std::string pattern, std::string outfix_, std::shared_ptr<File::Rule> rule_ = getMaxRules()) :
             InternalSet(piol_, pattern, outfix_, rule_)  {  }
    Set(Piol piol_, std::string pattern, std::shared_ptr<File::Rule> rule_ = getMaxRules()) :
             InternalSet(piol_, pattern, "", rule_)  {  }
    //For tests:
    Set(Piol piol_, std::shared_ptr<File::Rule> rule_ = getMaxRules()) :
             InternalSet(piol_, rule_)  {  }


    void sort(File::SortType type);
    void getMinMax(File::Meta m1, File::Meta m2, File::CoordElem * minmax);
};
}
#endif
