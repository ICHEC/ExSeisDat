////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_EXSEIS_SEGY_TRACE_HEADER_OFFSETS_HH
#define EXSEISDAT_EXSEIS_SEGY_TRACE_HEADER_OFFSETS_HH

#include <cstddef>

namespace exseis {
namespace piol {
namespace segy {

/// @brief SEG-Y Trace Header offsets
enum class Trace_header_offsets : size_t {
    /// int32_t. The trace sequence number in the Line.
    SeqNum = 1U,

    /// int32_t. The trace sequence number in SEG-Y File.
    SeqFNum = 5U,

    /// int32_t. The original field record number.
    ORF = 9U,

    /// int32_t. The trace number in the ORF.
    TORF = 13U,

    /// int32_t. The source energy number.
    ENSrcNum = 17U,

    /// int32_t. The trace number in the ensemble.
    SeqNumEns = 25U,

    /// int16_t. The trace identification number.
    TIC = 29U,

    /// int16_t. The number of traces vertically stacked.
    VStackCnt = 31U,

    /// int16_t. The number of traces horizontally stacked.
    HStackCnt = 33U,

    /// int32_t. The distance from source center to receiver centre.
    CDist = 37U,

    /// int32_t. The receiver group elevation.
    RcvElv = 41U,

    /// int32_t. The surface elevation at the source.
    SurfElvSrc = 45U,

    /// int32_t. The source depth below surface (opposite of above?).
    SrcDpthSurf = 49U,

    /// int32_t. The datum elevation for the receiver group.
    DtmElvRcv = 53U,

    /// int32_t. The datum elevation for the source.
    DtmElvSrc = 57U,

    /// int32_t. The water depth for the source.
    WtrDepSrc = 61U,

    /// int32_t. The water depth for the receive group.
    WtrDepRcv = 65U,

    /// int16_t. The scale coordinate for 41-68 (elevations + depths).
    ScaleElev = 69U,

    /// int16_t. The scale coordinate for 73-88 + 181-188
    ScaleCoord = 71U,

    /// int32_t. The X coordinate for the source
    x_src = 73U,

    /// int32_t. The Y coordinate for the source
    y_src = 77U,

    /// int32_t. The X coordinate for the receive group
    x_rcv = 81U,

    /// int32_t. The Y coordinate for the receive group
    y_rcv = 85U,

    /// int16_t. The uphole time at the source (ms).
    UpSrc = 95U,

    /// int16_t. The uphole time at the receive group (ms).
    UpRcv = 97U,

    /// int16_t. The number of samples in the trace.
    Ns = 115U,

    /// int16_t. The sample interval (us).
    sample_interval = 117U,

    /// int32_t. The X coordinate for the CMP
    xCmp = 181U,

    /// int32_t. The Y coordinate for the CMP
    yCmp = 185U,

    /// int32_t. The Inline grid point.
    il = 189U,

    /// int32_t. The Crossline grid point.
    xl = 193U,

    /// int32_t. The source nearest to the CDP.
    ShotNum = 197U,

    /// int16_t. The shot number scalar. (Explicitly says that 0 == 1)
    ShotScal = 201U,

    /// int16_t. The unit system used for trace values.
    ValMeas = 203U,

    /// int32_t. The transduction constant.
    TransConst = 205U,

    /// int16_t. The transduction exponent.
    TransExp = 209U,

    /// int16_t. The transduction units
    TransUnit = 211U,

    /// int16_t. Scalar for time measurements.
    TimeScal = 215U,

    /// int32_t. Source measurement.
    SrcMeas = 225U,

    /// int16_t. Source measurement exponent.
    SrcMeasExp = 229U
};

}  // namespace segy
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_EXSEIS_SEGY_TRACE_HEADER_OFFSETS_HH
