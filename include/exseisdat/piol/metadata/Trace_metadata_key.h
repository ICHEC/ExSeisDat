////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C API for \ref exseisdat/piol/metadata/Trace_metadata_key.hh
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_METADATA_TRACE_METADATA_KEY_H
#define EXSEISDAT_PIOL_METADATA_TRACE_METADATA_KEY_H

#include <stddef.h>

#ifdef __cplusplus
namespace exseis {
namespace piol {
inline namespace metadata {

extern "C" {
#endif  // __cplusplus


/// @name C API
///
/// @{

/// @brief C API for exseis::piol::metadata::Trace_metadata_key
/// @copydoc exseis::piol::metadata::Trace_metadata_key
typedef size_t exseis_Trace_metadata_key;


/// @brief C API or exseis::piol::metadata::Trace_metadata_key
/// @copydoc exseis::piol::metadata::Trace_metadata_key
enum {
    /// Copy all headers. Don't interpret them.
    exseis_meta_Copy,

    /// Local trace number.
    exseis_meta_ltn,

    /// Global trace number.
    exseis_meta_gtn,

    /// The trace number (line)
    exseis_meta_tnl,

    /// The Trace number (record)
    exseis_meta_tnr,

    /// The trace number (file)
    exseis_meta_tn,

    /// The trace number (ensemble)
    exseis_meta_tne,

    /// Number of samples in this trace
    exseis_meta_ns,

    /// The increment of this trace.
    exseis_meta_sample_interval,

    /// Trace identification code
    exseis_meta_Tic,

    /// Source Number
    exseis_meta_SrcNum,

    /// Shot number
    exseis_meta_ShotNum,

    /// Number of traces stacked for this trace (vertical)
    exseis_meta_VStack,

    /// Number of traces stacked for this trace (horizontal)
    exseis_meta_HStack,

    /// Distance from source to receiver
    exseis_meta_Offset,

    /// Receiver group elevation
    exseis_meta_RGElev,

    /// Source surface elevation
    exseis_meta_SSElev,

    /// Source depth
    exseis_meta_SDElev,

    /// The Water depth at source
    exseis_meta_WtrDepSrc,

    /// The Water depth at receiver
    exseis_meta_WtrDepRcv,

    /// The source x coordinate
    exseis_meta_x_src,

    /// The source y coordinate
    exseis_meta_y_src,

    /// The receiver x coordinate
    exseis_meta_x_rcv,

    /// The receiver y coordinate
    exseis_meta_y_rcv,

    /// The CMP x coordinate
    exseis_meta_xCmp,

    /// The CMP y coordinate
    exseis_meta_yCmp,

    /// The coordinate scalar value in a SEGY file
    exseis_meta_coordinate_scalar,

    /// The inline number
    exseis_meta_il,

    /// The crossline number
    exseis_meta_xl,

    /// Unit system for transduction constant
    exseis_meta_TransUnit,

    /// Unit system for traces
    exseis_meta_TraceUnit,

    // Non-standard

    /// The sum of the differences between sources and receivers of this trace
    /// and another
    exseis_meta_dsdr,

    // TODO: Don't add more of these, find out what they do and replace them
    // with real names

    /// Miscellaneous
    exseis_meta_Misc1,

    /// Miscellaneous
    exseis_meta_Misc2,

    /// Miscellaneous
    exseis_meta_Misc3,

    /// Miscellaneous
    exseis_meta_Misc4
};

/// @} C API

#ifdef __cplusplus
}  // extern "C"

}  // namespace metadata
}  // namespace piol
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_PIOL_METADATA_TRACE_METADATA_KEY_H
