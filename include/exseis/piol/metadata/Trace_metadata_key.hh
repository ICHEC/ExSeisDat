////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_PIOL_METADATA_TRACE_METADATA_KEY_HH
#define EXSEIS_PIOL_METADATA_TRACE_METADATA_KEY_HH

#include <cstddef>

namespace exseis {
inline namespace piol {
inline namespace metadata {


/// @brief The available trace parameters
///
enum class Trace_metadata_key : size_t {
    /// Copy all headers. Don't interpret them.
    Copy,

    /// Local trace number.
    ltn,

    /// Global trace number.
    gtn,

    /// The trace number (line)
    tnl,

    /// The Trace number (record)
    tnr,

    /// The trace number (file)
    tn,

    /// The trace number (ensemble)
    tne,

    /// Number of samples in this trace
    ns,

    /// The increment of this trace.
    sample_interval,

    /// Trace identification code
    Tic,

    /// Source Number
    SrcNum,

    /// Shot number
    ShotNum,

    /// Number of traces stacked for this trace (vertical)
    VStack,

    /// Number of traces stacked for this trace (horizontal)
    HStack,

    /// Distance from source to receiver
    Offset,

    /// Receiver group elevation
    RGElev,

    /// Source surface elevation
    SSElev,

    /// Source depth
    SDElev,

    /// The Water depth at source
    WtrDepSrc,

    /// The Water depth at receiver
    WtrDepRcv,

    /// The source x coordinate
    x_src,

    /// The source y coordinate
    y_src,

    /// The receiver x coordinate
    x_rcv,

    /// The receiver y coordinate
    y_rcv,

    /// The CMP x coordinate
    xCmp,

    /// The CMP y coordinate
    yCmp,

    /// The coordinate scalar in a SEGY file
    coordinate_scalar,

    /// The inline number
    il,

    /// The crossline number
    xl,

    /// Unit system for transduction constant
    TransUnit,

    /// Unit system for traces
    TraceUnit,

    // Non-standard

    /// The sum of the differences between sources and receivers of this trace
    /// and another
    dsdr,

    // TODO: Don't add more of these, find out what they do and replace them
    // with real names

    /// Miscellaneous
    Misc1,

    /// Miscellaneous
    Misc2,

    /// Miscellaneous
    Misc3,

    /// Miscellaneous
    Misc4
};


}  // namespace metadata
}  // namespace piol
}  // namespace exseis

#endif  // EXSEIS_PIOL_METADATA_TRACE_METADATA_KEY_HH
