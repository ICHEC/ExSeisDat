////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date December 2016
/// @details This file will be called by C++ and C11 code as well as internal
///          code and external facing library headers.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_SHARE_API_HH
#define EXSEISDAT_PIOL_SHARE_API_HH

#include <stddef.h>

/// A C-compatible typename for the type used for trace parameters, i.e. META
/// values.
typedef size_t PIOL_Meta;

#ifdef __cplusplus
namespace PIOL {
/// @copydoc PIOL_Meta
typedef PIOL_Meta Meta;
}  // namespace PIOL
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


/*! SEG-Y Trace Header offsets
 */
typedef size_t PIOL_Tr;

#ifdef __cplusplus
namespace PIOL {
/// @copydoc PIOL_Tr
typedef PIOL_Tr Tr;
}  // namespace PIOL
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


/*! An enum class of the different types of sorting operation.
 */
typedef size_t PIOL_SortType;

#ifdef __cplusplus
namespace PIOL {
/// @copydoc PIOL_SortType
typedef PIOL_SortType SortType;
}  // namespace PIOL
#endif  // __cplusplus

/// An enumeration of the different types of sorting operation.
enum {
    /// Sort by source x, source y, receiver x, receiver y
    PIOL_SORTTYPE_SrcRcv = 35,

    /// Sort by source x, source y, calcuated offset
    PIOL_SORTTYPE_SrcOff = 36,

    /// Sort by source x, source y, offset read from file
    PIOL_SORTTYPE_SrcROff = 37,

    /// Sort by receiver x, receiver y, calculate offset
    PIOL_SORTTYPE_RcvOff = 38,

    /// Sort by receiver x, receiver y, offset read from file
    PIOL_SORTTYPE_RcvROff = 39,

    /// Sort by inline, crossline, calculated offset
    PIOL_SORTTYPE_LineOff = 40,

    /// Sort by inline, crossline, offset read from file
    PIOL_SORTTYPE_LineROff = 41,

    /// Sort by calculated offset, inline, crossline
    PIOL_SORTTYPE_OffLine = 42,

    /// Sort by offset read from file, inline, crossline
    PIOL_SORTTYPE_ROffLine = 43
};


/*! An enum class of the different types of tapering windows.
 */
typedef size_t PIOL_TaperType;

#ifdef __cplusplus
namespace PIOL {
/// @copydoc PIOL_TaperType
typedef PIOL_TaperType TaperType;
}  // namespace PIOL
#endif  // __cplusplus

/// An enumeration of the different types of tapering windows.
enum {
    /// Taper using a linear ramp
    PIOL_TAPERTYPE_Linear = 44,

    /// Taper using a cos ramp
    PIOL_TAPERTYPE_Cos = 45,

    /// Taper using a cos^2 ramp
    PIOL_TAPERTYPE_CosSqr = 46
};


/*! An enum class of the different types of automatic gain control functions.
 */
typedef size_t PIOL_AGCType;

#ifdef __cplusplus
namespace PIOL {
/// @copydoc PIOL_AGCType
typedef PIOL_AGCType AGCType;
}  // namespace PIOL
#endif  // __cplusplus

/// An enumeration of the different types of automatic gain control functions.
enum {
    /// AGC using RMS with rectangular window
    PIOL_AGCTYPE_RMS = 47,

    /// AGC using RMS with triangular window
    PIOL_AGCTYPE_RMSTri = 48,

    /// AGC using mean absolute value with rectangular window
    PIOL_AGCTYPE_MeanAbs = 49,

    /// AGC using the median value
    PIOL_AGCTYPE_Median = 50
};


#ifdef __cplusplus
namespace PIOL {

/*! An enum class of the different types of filters.
 */
enum class FltrType : int {
    /// Create Lowpass IIR Butterworth filter
    Lowpass,

    /// Create Highpass IIR Butterworth filter
    Highpass,

    /// Create Bandpass IIR Butterworth filter
    Bandpass,

    /// Create Bandstop IIR Butterworth filter
    Bandstop
};


/*! An enum class of the different types of filtering domains.
 */
enum class FltrDmn : int {
    /// Filter in time domain
    Time,

    /// Filter in frequency domain
    Freq
};


/*! An enum class of the different types of trace padding functions.
 */
enum class PadType : int {
    /// Pad using zeros
    Zero,

    /// Pad using reflection of trace
    Symmetric,

    /// Pad using closest value in trace
    Replicate,

    /// Pad using values from other end of trace
    Cyclic
};

}  // namespace PIOL
#endif  // __cplusplus


#endif  // EXSEISDAT_PIOL_SHARE_API_HH
