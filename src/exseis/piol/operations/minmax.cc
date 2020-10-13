////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////

#include "exseis/piol/operations/minmax.hh"
#include "exseis/utils/types/typedefs.hh"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>

namespace exseis {
inline namespace piol {
inline namespace operations {

void get_min_max(
    const Communicator& communicator,
    size_t offset,
    size_t number_of_traces,
    Trace_metadata_key m1,
    Trace_metadata_key m2,
    const Trace_metadata& trace_metadata,
    CoordElem* minmax)
{
    std::vector<Trace_metadata> trace_metadata_list;
    // TODO: Just add the two meta options to the rules with defaults?
    for (size_t i = 0; i < number_of_traces; i++) {
        trace_metadata_list.emplace_back(trace_metadata.entry_types(), 1LU);
        trace_metadata_list.back().copy_entries(0, trace_metadata, i);
    }

    get_min_max<Trace_metadata>(
        communicator, offset, number_of_traces, trace_metadata_list.data(),
        [m1](const Trace_metadata& a) -> Floating_point {
            return a.get_floating_point(0LU, m1);
        },
        [m2](const Trace_metadata& a) -> Floating_point {
            return a.get_floating_point(0LU, m2);
        },
        minmax);
}

}  // namespace operations
}  // namespace piol
}  // namespace exseis
