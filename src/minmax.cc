/*******************************************************************************************//*!
 *   @file
 *   @author Cathal O Broin - cathal@ichec.ie - first commit
 *   @copyright TBD. Do not distribute
 *   @date November 2016
 *   @brief The Sort Operation
 *   @details The algorithm used is a nearest neighbour approach where at each iteration
 *   the lowest valued metadata entries are moved to adjacent processes with a lower rank and a
 *   sort is performed. After the sort the highest entries are moved again to the process with a
 *   higher rank. If each process has the same traces it started off with, the sort
 *   is complete.
*//*******************************************************************************************/
#include <algorithm>
#include <iterator>
#include <functional>

#include "global.hh"
#include "file/dynsegymd.hh"
#include "ops/minmax.hh"

namespace PIOL { namespace File {
void getMinMax(ExSeisPIOL * piol, size_t offset, size_t lnt, Meta m1, Meta m2, const Param * prm, CoordElem * minmax)
{
    std::vector<Param> vprm;
    //TODO: Just add the two meta options to the rules with defaults?
    for (size_t i = 0; i < lnt; i++)
    {
        vprm.emplace_back(prm->r, 1LU);
        cpyPrm(i, prm, 0, &vprm.back());
    }

    getMinMax<Param>(piol, offset, lnt, vprm.data(), [m1](const Param & a) -> geom_t { return getPrm<geom_t>(0LU, m1, &a); },
                                                     [m2](const Param & a) -> geom_t { return getPrm<geom_t>(0LU, m2, &a); }, minmax);
}
}}
