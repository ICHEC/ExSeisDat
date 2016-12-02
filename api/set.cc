/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date November 2016
 *   \brief The Set layer interface
*//*******************************************************************************************/
#include "set.hh"
#include "file/dynsegymd.hh"
namespace PIOL {
std::shared_ptr<File::Rule> getMaxRules(void)
{
    auto rule = std::make_shared<File::Rule>(true, true, true);
    rule->addLong(File::Meta::Misc1, File::Tr::TransConst);
    rule->addShort(File::Meta::Misc2, File::Tr::TransExp);
    //Override the default behaviour of ShotNum
    rule->addLong(File::Meta::ShotNum, File::Tr::ShotNum);
    rule->addShort(File::Meta::Misc3, File::Tr::ShotScal);
    return rule;
}


void Set::sort(File::SortType type)
{
    InternalSet::sort(File::getComp(type));
}


void Set::getMinMax(File::Meta m1, File::Meta m2, File::CoordElem * minmax)
{
    InternalSet::getMinMax([m1](const File::Param & a) -> geom_t { return File::getPrm<geom_t>(0U, m1, &a); },
                           [m2](const File::Param & a) -> geom_t { return File::getPrm<geom_t>(0U, m2, &a); }, minmax);

}
}
