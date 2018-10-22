////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/operations/minmax.hh"
#include "exseisdat/utils/typedefs.hh"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>

namespace exseis {
namespace piol {

void get_min_max(
  ExSeisPIOL* piol,
  size_t offset,
  size_t lnt,
  Meta m1,
  Meta m2,
  const Trace_metadata& prm,
  CoordElem* minmax)
{
    std::vector<Trace_metadata> vprm;
    // TODO: Just add the two meta options to the rules with defaults?
    for (size_t i = 0; i < lnt; i++) {
        vprm.emplace_back(prm.rules, 1LU);
        vprm.back().copy_entries(0, prm, i);
    }

    get_min_max<Trace_metadata>(
      piol, offset, lnt, vprm.data(),
      [m1](const Trace_metadata& a) -> exseis::utils::Floating_point {
          return a.get_floating_point(0LU, m1);
      },
      [m2](const Trace_metadata& a) -> exseis::utils::Floating_point {
          return a.get_floating_point(0LU, m2);
      },
      minmax);
}

}  // namespace piol
}  // namespace exseis
