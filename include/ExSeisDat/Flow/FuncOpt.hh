////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_FUNCOPT_HH
#define EXSEISDAT_FLOW_FUNCOPT_HH

#include <cstddef>

namespace exseis {
namespace Flow {

/*! Enum class for the various function options.
 */
enum class FuncOpt : size_t {
    // Data type dependencies

    /// Metadata required to be read.
    NeedMeta,

    /// Trace values required to be read.
    NeedTrcVal,

    // Modification level:

    /// Traces are added by the operation.
    AddTrc,

    /// Traces are deleted by the operation.
    DelTrc,

    /// Trace values are modified by the operation.
    ModTrcVal,

    /// Trace lengths are modified by the operation.
    ModTrcLen,

    /// Metadata values are modified by the operation.
    ModMetaVal,

    /// Traces are reordered by the operation.
    ReorderTrc,

    // Modification dependencies

    /// There is a dependency on the number of traces.
    DepTrcCnt,

    /// There is a dependency on the order of traces.
    DepTrcOrder,

    /// There is a dependency on the value of traces.
    DepTrcVal,

    /// There is a dependency on the metadata values.
    DepMetaVal,

    // Comms level:

    /// Each output trace requires info from one input trace.
    SingleTrace,

    /// Each output trace requires info from traces in the same gather.
    Gather,

    /// Each output trace requires info from all traces in a subset of files.
    SubSetOnly,

    /// Each output trace requires info from all traces in the set.
    AllTraces,

    /// Management of traces is complicated and custom.
    OwnIO
};

}  // namespace Flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_FUNCOPT_HH
