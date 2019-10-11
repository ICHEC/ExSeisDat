#include "mockgetminmax.hh"

namespace exseis {
namespace piol {
inline namespace operations {

void get_min_max(
    ExSeisPIOL* piol,
    size_t offset,
    size_t sz,
    Trace_metadata_key m1,
    Trace_metadata_key m2,
    const Trace_metadata& prm,
    CoordElem* minmax)
{
    return mock_get_min_max().get_min_max(
        piol, offset, sz, m1, m2, prm, minmax);
}

}  // namespace operations
}  // namespace piol
}  // namespace exseis
