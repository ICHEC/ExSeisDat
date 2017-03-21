/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date November 2016
 *   \brief The Set layer interface
*//*******************************************************************************************/
#include "global.hh"
#include "set.hh"
#include "file/dynsegymd.hh"
namespace PIOL {
std::shared_ptr<File::Rule> getMaxRules(void)
{
    auto rule = std::make_shared<File::Rule>(true, true, true);
    rule->addLong(Meta::Misc1, File::Tr::TransConst);
    rule->addShort(Meta::Misc2, File::Tr::TransExp);
    //Override the default behaviour of ShotNum
    rule->addLong(Meta::ShotNum, File::Tr::ShotNum);
    rule->addShort(Meta::Misc3, File::Tr::ShotScal);
    return rule;
}

void Set::sort(SortType type)
{
    InternalSet::sort(File::getComp(type));
}

void Set::getMinMax(Meta m1, Meta m2, CoordElem * minmax)
{
    InternalSet::getMinMax([m1](const File::Param & a) -> geom_t { return File::getPrm<geom_t>(0U, m1, &a); },
                           [m2](const File::Param & a) -> geom_t { return File::getPrm<geom_t>(0U, m2, &a); }, minmax);

}
}
