////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_META_HH
#define EXSEISDAT_PIOL_META_HH

#include <cstddef>

namespace exseis {
namespace piol {


/// The available trace parameters
///
enum class Meta : size_t {
    /// Copy all headers. Don't interpret them.
    Copy = 0,

    /// Local trace number.
    ltn = 1,

    /// Global trace number.
    gtn = 2,

    /// The trace number (line)
    tnl = 3,

    /// The Trace number (record)
    tnr = 4,

    /// The trace number (file)
    tn = 5,

    /// The trace number (ensemble)
    tne = 6,

    /// Number of samples in this trace
    ns = 7,

    /// The increment of this trace.
    sample_interval = 8,

    /// Trace identification code
    Tic = 9,

    /// Source Number
    SrcNum = 10,

    /// Shot number
    ShotNum = 11,

    /// Number of traces stacked for this trace (vertical)
    VStack = 12,

    /// Number of traces stacked for this trace (horizontal)
    HStack = 13,

    /// Distance from source to receiver
    Offset = 14,

    /// Receiver group elevation
    RGElev = 15,

    /// Source surface elevation
    SSElev = 16,

    /// Source depth
    SDElev = 17,

    /// The Water depth at source
    WtrDepSrc = 18,

    /// The Water depth at receiver
    WtrDepRcv = 19,

    /// The source x coordinate
    x_src = 20,

    /// The source y coordinate
    y_src = 21,

    /// The receiver x coordinate
    x_rcv = 22,

    /// The receiver y coordinate
    y_rcv = 23,

    /// The CMP x coordinate
    xCmp = 24,

    /// The CMP y coordinate
    yCmp = 25,

    /// The inline number
    il = 26,

    /// The crossline number
    xl = 27,

    /// Unit system for transduction constant
    TransUnit = 28,

    /// Unit system for traces
    TraceUnit = 29,

    // Non-standard

    /// The sum of the differences between sources and receivers of this trace
    /// and another
    dsdr = 30,

    // TODO: Don't add more of these, find out what they do and replace them
    // with real names

    /// Miscellaneous
    Misc1 = 31,

    /// Miscellaneous
    Misc2 = 32,

    /// Miscellaneous
    Misc3 = 33,

    /// Miscellaneous
    Misc4 = 34
};


}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_META_HH
