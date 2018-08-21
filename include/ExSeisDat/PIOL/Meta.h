////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_META_H
#define EXSEISDAT_PIOL_META_H

#include <stddef.h>

/// A C-compatible typename for the type used for trace parameters, i.e. META
/// values.
typedef size_t PIOL_Meta;

#ifdef __cplusplus
namespace exseis {
namespace PIOL {
/// @copydoc PIOL_Meta
typedef PIOL_Meta Meta;
}  // namespace PIOL
}  // namespace exseis
#endif  // __cplusplus

/*! The available trace parameters
 */
enum {
    /// Copy all headers. Don't interpret them.
    PIOL_META_COPY = 0,

    /// Local trace number.
    PIOL_META_ltn = 1,

    /// Global trace number.
    PIOL_META_gtn = 2,

    /// The trace number (line)
    PIOL_META_tnl = 3,

    /// The Trace number (record)
    PIOL_META_tnr = 4,

    /// The trace number (file)
    PIOL_META_tn = 5,

    /// The trace number (ensemble)
    PIOL_META_tne = 6,

    /// Number of samples in this trace
    PIOL_META_ns = 7,

    /// The increment of this trace.
    PIOL_META_inc = 8,

    /// Trace identification code
    PIOL_META_Tic = 9,

    /// Source Number
    PIOL_META_SrcNum = 10,

    /// Shot number
    PIOL_META_ShotNum = 11,

    /// Number of traces stacked for this trace (vertical)
    PIOL_META_VStack = 12,

    /// Number of traces stacked for this trace (horizontal)
    PIOL_META_HStack = 13,

    /// Distance from source to receiver
    PIOL_META_Offset = 14,

    /// Receiver group elevation
    PIOL_META_RGElev = 15,

    /// Source surface elevation
    PIOL_META_SSElev = 16,

    /// Source depth
    PIOL_META_SDElev = 17,

    /// The Water depth at source
    PIOL_META_WtrDepSrc = 18,

    /// The Water depth at receiver
    PIOL_META_WtrDepRcv = 19,

    /// The source x coordinate
    PIOL_META_xSrc = 20,

    /// The source y coordinate
    PIOL_META_ySrc = 21,

    /// The receiver x coordinate
    PIOL_META_xRcv = 22,

    /// The receiver y coordinate
    PIOL_META_yRcv = 23,

    /// The CMP x coordinate
    PIOL_META_xCmp = 24,

    /// The CMP y coordinate
    PIOL_META_yCmp = 25,

    /// The inline number
    PIOL_META_il = 26,

    /// The crossline number
    PIOL_META_xl = 27,

    /// Unit system for transduction constant
    PIOL_META_TransUnit = 28,

    /// Unit system for traces
    PIOL_META_TraceUnit = 29,

    // Non-standard

    /// The sum of the differences between sources and receivers of this trace
    /// and
    /// another
    PIOL_META_dsdr = 30,

    // TODO: Don't add more of these, find out what they do and replace them
    // with real names

    /// Miscellaneous
    PIOL_META_Misc1 = 31,

    /// Miscellaneous
    PIOL_META_Misc2 = 32,

    /// Miscellaneous
    PIOL_META_Misc3 = 33,

    /// Miscellaneous
    PIOL_META_Misc4 = 34
};

#endif  // EXSEISDAT_PIOL_META_H
