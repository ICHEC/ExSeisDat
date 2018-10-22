#include "mockgetminmax.hh"

namespace exseis {
namespace piol {

void get_min_max(
  ExSeisPIOL* piol,
  size_t offset,
  size_t sz,
  Meta m1,
  Meta m2,
  const Trace_metadata& prm,
  CoordElem* minmax)
{
    return mock_get_min_max().get_min_max(
      piol, offset, sz, m1, m2, prm, minmax);
}

}  // namespace piol
}  // namespace exseis
