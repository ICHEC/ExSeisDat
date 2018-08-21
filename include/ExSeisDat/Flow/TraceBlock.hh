////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_TRACEBLOCK_HH
#define EXSEISDAT_FLOW_TRACEBLOCK_HH

#include "ExSeisDat/PIOL/Param.h"
#include "ExSeisDat/utils/typedefs.h"

#include <cstddef>
#include <memory>
#include <vector>

namespace exseis {
namespace Flow {

using namespace exseis::utils::typedefs;

/*! The structure for holding all trace data.
 */
struct TraceBlock {
    /// Number of traces
    size_t nt;

    /// Number of samples per trace
    size_t ns;

    /// Increment between trace samples
    exseis::utils::Floating_point inc;

    /// Gather number (if applicable)
    size_t gNum;

    /// Number of gathers (if applicable)
    size_t numG;

    /// unique pointer to parameter structure (if applicable)
    std::unique_ptr<exseis::PIOL::Param> prm;

    /// traces (if applicable)
    std::vector<exseis::utils::Trace_value> trc;
};

}  // namespace Flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_TRACEBLOCK_HH
