////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C API for \ref exseisdat/piol/segy/Trace_header_offsets.hh
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_EXSEIS_SEGY_TRACE_HEADER_OFFSETS_H
#define EXSEISDAT_EXSEIS_SEGY_TRACE_HEADER_OFFSETS_H

#include <stddef.h>

#ifdef __cplusplus
namespace exseis {
namespace piol {
namespace segy {

extern "C" {
#endif  // __cplusplus

/// @name C API
///
/// @{

/// @brief C API for \ref exseis::piol::segy::Trace_header_offsets
/// @copydoc exseis::piol::segy::Trace_header_offsets
typedef size_t exseis_segy_Trace_header_offsets;

/// @brief C API for \ref exseis::piol::segy::Trace_header_offsets
/// @copydoc exseis::piol::segy::Trace_header_offsets
enum {
    /// int32_t. The trace sequence number in the Line.
    exseis_segy_tr_SeqNum = 1U,

    /// int32_t. The trace sequence number in SEG-Y File.
    exseis_segy_tr_SeqFNum = 5U,

    /// int32_t. The original field record number.
    exseis_segy_tr_ORF = 9U,

    /// int32_t. The trace number in the ORF.
    exseis_segy_tr_TORF = 13U,

    /// int32_t. The source energy number.
    exseis_segy_tr_ENSrcNum = 17U,

    /// int32_t. The trace number in the ensemble.
    exseis_segy_tr_SeqNumEns = 25U,

    /// int16_t. The trace identification number.
    exseis_segy_tr_TIC = 29U,

    /// int16_t. The number of traces vertically stacked.
    exseis_segy_tr_VStackCnt = 31U,

    /// int16_t. The number of traces horizontally stacked.
    exseis_segy_tr_HStackCnt = 33U,

    /// int32_t. The distance from source center to receiver centre.
    exseis_segy_tr_CDist = 37U,

    /// int32_t. The receiver group elevation.
    exseis_segy_tr_RcvElv = 41U,

    /// int32_t. The surface elevation at the source.
    exseis_segy_tr_SurfElvSrc = 45U,

    /// int32_t. The source depth below surface (opposite of above?).
    exseis_segy_tr_SrcDpthSurf = 49U,

    /// int32_t. The datum elevation for the receiver group.
    exseis_segy_tr_DtmElvRcv = 53U,

    /// int32_t. The datum elevation for the source.
    exseis_segy_tr_DtmElvSrc = 57U,

    /// int32_t. The water depth for the source.
    exseis_segy_tr_WtrDepSrc = 61U,

    /// int32_t. The water depth for the receive group.
    exseis_segy_tr_WtrDepRcv = 65U,

    /// int16_t. The scale coordinate for 41-68 (elevations + depths).
    exseis_segy_tr_ScaleElev = 69U,

    /// int16_t. The scale coordinate for 73-88 + 181-188
    exseis_segy_tr_ScaleCoord = 71U,

    /// int32_t. The X coordinate for the source
    exseis_segy_tr_x_src = 73U,

    /// int32_t. The Y coordinate for the source
    exseis_segy_tr_y_src = 77U,

    /// int32_t. The X coordinate for the receive group
    exseis_segy_tr_x_rcv = 81U,

    /// int32_t. The Y coordinate for the receive group
    exseis_segy_tr_y_rcv = 85U,

    /// int16_t. The uphole time at the source (ms).
    exseis_segy_tr_UpSrc = 95U,

    /// int16_t. The uphole time at the receive group (ms).
    exseis_segy_tr_UpRcv = 97U,

    /// int16_t. The number of samples in the trace.
    exseis_segy_tr_Ns = 115U,

    /// int16_t. The sample interval (us).
    exseis_segy_tr_sample_interval = 117U,

    /// int32_t. The X coordinate for the CMP
    exseis_segy_tr_xCmp = 181U,

    /// int32_t. The Y coordinate for the CMP
    exseis_segy_tr_yCmp = 185U,

    /// int32_t. The Inline grid point.
    exseis_segy_tr_il = 189U,

    /// int32_t. The Crossline grid point.
    exseis_segy_tr_xl = 193U,

    /// int32_t. The source nearest to the CDP.
    exseis_segy_tr_ShotNum = 197U,

    /// int16_t. The shot number scalar. (Explicitly says that 0 == 1)
    exseis_segy_tr_ShotScal = 201U,

    /// int16_t. The unit system used for trace values.
    exseis_segy_tr_ValMeas = 203U,

    /// int32_t. The transduction constant.
    exseis_segy_tr_TransConst = 205U,

    /// int16_t. The transduction exponent.
    exseis_segy_tr_TransExp = 209U,

    /// int16_t. The transduction units
    exseis_segy_tr_TransUnit = 211U,

    /// int16_t. Scalar for time measurements.
    exseis_segy_tr_TimeScal = 215U,

    /// int32_t. Source measurement.
    exseis_segy_tr_SrcMeas = 225U,

    /// int16_t. Source measurement exponent.
    exseis_segy_tr_SrcMeasExp = 229U
};

/// @} C API

#ifdef __cplusplus
}  // extern "C"

}  // namespace segy
}  // namespace piol
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_EXSEIS_SEGY_TRACE_HEADER_OFFSETS_H
