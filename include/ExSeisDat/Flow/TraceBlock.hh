////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_TRACEBLOCK_HH
#define EXSEISDAT_FLOW_TRACEBLOCK_HH

#include "ExSeisDat/PIOL/Param.h"
#include "ExSeisDat/PIOL/typedefs.h"

#include <cstddef>
#include <memory>
#include <vector>

namespace PIOL {

/*! The structure for holding all trace data.
 */
struct TraceBlock {
    /// Number of traces
    size_t nt;

    /// Number of samples per trace
    size_t ns;

    /// Increment between trace samples
    geom_t inc;

    /// Gather number (if applicable)
    size_t gNum;

    /// Number of gathers (if applicable)
    size_t numG;

    /// unique pointer to parameter structure (if applicable)
    std::unique_ptr<Param> prm;

    /// traces (if applicable)
    std::vector<trace_t> trc;
};

}  // namespace PIOL

#endif  // EXSEISDAT_FLOW_TRACEBLOCK_HH
