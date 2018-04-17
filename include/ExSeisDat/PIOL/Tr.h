////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_TR_H
#define EXSEISDAT_PIOL_TR_H

#include <stddef.h>

/*! SEG-Y Trace Header offsets
 */
typedef size_t PIOL_Tr;

#ifdef __cplusplus
namespace exseis {
namespace PIOL {
/// @copydoc PIOL_Tr
typedef PIOL_Tr Tr;
}  // namespace PIOL
}  // namespace exseis
#endif  // __cplusplus


/*! SEG-Y Trace Header offsets
 */
enum {
    /// int32_t. The trace sequence number in the Line.
    PIOL_TR_SeqNum = 1U,

    /// int32_t. The trace sequence number in SEG-Y File.
    PIOL_TR_SeqFNum = 5U,

    /// int32_t. The original field record number.
    PIOL_TR_ORF = 9U,

    /// int32_t. The trace number in the ORF.
    PIOL_TR_TORF = 13U,

    /// int32_t. The source energy number.
    PIOL_TR_ENSrcNum = 17U,

    /// int32_t. The trace number in the ensemble.
    PIOL_TR_SeqNumEns = 25U,

    /// int16_t. The trace identification number.
    PIOL_TR_TIC = 29U,

    /// int16_t. The number of traces vertically stacked.
    PIOL_TR_VStackCnt = 31U,

    /// int16_t. The number of traces horizontally stacked.
    PIOL_TR_HStackCnt = 33U,

    /// int32_t. The distance from source center to receiver centre.
    PIOL_TR_CDist = 37U,

    /// int32_t. The receiver group elevation.
    PIOL_TR_RcvElv = 41U,

    /// int32_t. The surface elevation at the source.
    PIOL_TR_SurfElvSrc = 45U,

    /// int32_t. The source depth below surface (opposite of above?).
    PIOL_TR_SrcDpthSurf = 49U,

    /// int32_t. The datum elevation for the receiver group.
    PIOL_TR_DtmElvRcv = 53U,

    /// int32_t. The datum elevation for the source.
    PIOL_TR_DtmElvSrc = 57U,

    /// int32_t. The water depth for the source.
    PIOL_TR_WtrDepSrc = 61U,

    /// int32_t. The water depth for the receive group.
    PIOL_TR_WtrDepRcv = 65U,

    /// int16_t. The scale coordinate for 41-68 (elevations + depths).
    PIOL_TR_ScaleElev = 69U,

    /// int16_t. The scale coordinate for 73-88 + 181-188
    PIOL_TR_ScaleCoord = 71U,

    /// int32_t. The X coordinate for the source
    PIOL_TR_xSrc = 73U,

    /// int32_t. The Y coordinate for the source
    PIOL_TR_ySrc = 77U,

    /// int32_t. The X coordinate for the receive group
    PIOL_TR_xRcv = 81U,

    /// int32_t. The Y coordinate for the receive group
    PIOL_TR_yRcv = 85U,

    /// int16_t. The uphole time at the source (ms).
    PIOL_TR_UpSrc = 95U,

    /// int16_t. The uphole time at the receive group (ms).
    PIOL_TR_UpRcv = 97U,

    /// int16_t. The number of samples in the trace.
    PIOL_TR_Ns = 115U,

    /// int16_t. The sample interval (us).
    PIOL_TR_Inc = 117U,

    /// int32_t. The X coordinate for the CMP
    PIOL_TR_xCmp = 181U,

    /// int32_t. The Y coordinate for the CMP
    PIOL_TR_yCmp = 185U,

    /// int32_t. The Inline grid point.
    PIOL_TR_il = 189U,

    /// int32_t. The Crossline grid point.
    PIOL_TR_xl = 193U,

    /// int32_t. The source nearest to the CDP.
    PIOL_TR_ShotNum = 197U,

    /// int16_t. The shot number scalar. (Explicitly says that 0 == 1)
    PIOL_TR_ShotScal = 201U,

    /// int16_t. The unit system used for trace values.
    PIOL_TR_ValMeas = 203U,

    /// int32_t. The transduction constant.
    PIOL_TR_TransConst = 205U,

    /// int16_t. The transduction exponent.
    PIOL_TR_TransExp = 209U,

    /// int16_t. The transduction units
    PIOL_TR_TransUnit = 211U,

    /// int16_t. Scalar for time measurements.
    PIOL_TR_TimeScal = 215U,

    /// int32_t. Source measurement.
    PIOL_TR_SrcMeas = 225U,

    /// int16_t. Source measurement exponent.
    PIOL_TR_SrcMeasExp = 229U
};

#endif  // EXSEISDAT_PIOL_TR_H
