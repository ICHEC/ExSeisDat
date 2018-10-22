////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_TRACEBLOCK_HH
#define EXSEISDAT_FLOW_TRACEBLOCK_HH

#include "exseisdat/piol/Trace_metadata.hh"
#include "exseisdat/utils/typedefs.hh"

#include <cstddef>
#include <memory>
#include <vector>

namespace exseis {
namespace flow {

using namespace exseis::utils::typedefs;

/*! The structure for holding all trace data.
 */
struct TraceBlock {
    /// Number of traces
    size_t nt;

    /// Number of samples per trace
    size_t ns;

    /// Interval between trace samples
    exseis::utils::Floating_point sample_interval;

    /// Gather number (if applicable)
    size_t gather_number;

    /// Number of gathers (if applicable)
    size_t number_of_gathers;

    /// unique pointer to parameter structure (if applicable)
    std::unique_ptr<exseis::piol::Trace_metadata> prm;

    /// traces (if applicable)
    std::vector<exseis::utils::Trace_value> trc;
};

}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_TRACEBLOCK_HH
