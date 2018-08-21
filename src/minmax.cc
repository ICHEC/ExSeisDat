////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/operations/minmax.h"
#include "ExSeisDat/PIOL/param_utils.hh"
#include "ExSeisDat/utils/typedefs.h"

#include <algorithm>
#include <functional>
#include <iterator>

namespace exseis {
namespace PIOL {

void getMinMax(
  ExSeisPIOL* piol,
  size_t offset,
  size_t lnt,
  Meta m1,
  Meta m2,
  const Param* prm,
  CoordElem* minmax)
{
    std::vector<Param> vprm;
    // TODO: Just add the two meta options to the rules with defaults?
    for (size_t i = 0; i < lnt; i++) {
        vprm.emplace_back(prm->r, 1LU);
        param_utils::cpyPrm(i, prm, 0, &vprm.back());
    }

    getMinMax<Param>(
      piol, offset, lnt, vprm.data(),
      [m1](const Param& a) -> exseis::utils::Floating_point {
          return param_utils::getPrm<exseis::utils::Floating_point>(
            0LU, m1, &a);
      },
      [m2](const Param& a) -> exseis::utils::Floating_point {
          return param_utils::getPrm<exseis::utils::Floating_point>(
            0LU, m2, &a);
      },
      minmax);
}

}  // namespace PIOL
}  // namespace exseis
