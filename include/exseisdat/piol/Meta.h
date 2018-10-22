////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C API for \ref exseisdat/piol/Meta.hh
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_META_H
#define EXSEISDAT_PIOL_META_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


/// @brief C API or exseis::piol::Meta
/// @copydoc exseis::piol::Meta
typedef size_t exseis_Meta;


/// @brief C API or exseis::piol::Meta
/// @copydoc exseis::piol::Meta
enum {
    /// Copy all headers. Don't interpret them.
    exseis_meta_Copy = 0,

    /// Local trace number.
    exseis_meta_ltn = 1,

    /// Global trace number.
    exseis_meta_gtn = 2,

    /// The trace number (line)
    exseis_meta_tnl = 3,

    /// The Trace number (record)
    exseis_meta_tnr = 4,

    /// The trace number (file)
    exseis_meta_tn = 5,

    /// The trace number (ensemble)
    exseis_meta_tne = 6,

    /// Number of samples in this trace
    exseis_meta_ns = 7,

    /// The increment of this trace.
    exseis_meta_sample_interval = 8,

    /// Trace identification code
    exseis_meta_Tic = 9,

    /// Source Number
    exseis_meta_SrcNum = 10,

    /// Shot number
    exseis_meta_ShotNum = 11,

    /// Number of traces stacked for this trace (vertical)
    exseis_meta_VStack = 12,

    /// Number of traces stacked for this trace (horizontal)
    exseis_meta_HStack = 13,

    /// Distance from source to receiver
    exseis_meta_Offset = 14,

    /// Receiver group elevation
    exseis_meta_RGElev = 15,

    /// Source surface elevation
    exseis_meta_SSElev = 16,

    /// Source depth
    exseis_meta_SDElev = 17,

    /// The Water depth at source
    exseis_meta_WtrDepSrc = 18,

    /// The Water depth at receiver
    exseis_meta_WtrDepRcv = 19,

    /// The source x coordinate
    exseis_meta_x_src = 20,

    /// The source y coordinate
    exseis_meta_y_src = 21,

    /// The receiver x coordinate
    exseis_meta_x_rcv = 22,

    /// The receiver y coordinate
    exseis_meta_y_rcv = 23,

    /// The CMP x coordinate
    exseis_meta_xCmp = 24,

    /// The CMP y coordinate
    exseis_meta_yCmp = 25,

    /// The inline number
    exseis_meta_il = 26,

    /// The crossline number
    exseis_meta_xl = 27,

    /// Unit system for transduction constant
    exseis_meta_TransUnit = 28,

    /// Unit system for traces
    exseis_meta_TraceUnit = 29,

    // Non-standard

    /// The sum of the differences between sources and receivers of this trace
    /// and another
    exseis_meta_dsdr = 30,

    // TODO: Don't add more of these, find out what they do and replace them
    // with real names

    /// Miscellaneous
    exseis_meta_Misc1 = 31,

    /// Miscellaneous
    exseis_meta_Misc2 = 32,

    /// Miscellaneous
    exseis_meta_Misc3 = 33,

    /// Miscellaneous
    exseis_meta_Misc4 = 34
};

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // EXSEISDAT_PIOL_META_H
