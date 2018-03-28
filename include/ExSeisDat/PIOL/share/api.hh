////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date December 2016
/// @details This file will be called by C++ and C11 code as well as internal
///          code and external facing library headers.
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLSHAREAPI_INCLUDE_GUARD
#define PIOLSHAREAPI_INCLUDE_GUARD

#include "ExSeisDat/PIOL/anc/global.hh"

/// A C-compatible typename for the type used for trace parameters, i.e. META
/// values.
typedef size_t PIOL_Meta;

#ifdef __cplusplus
namespace PIOL {
/// A C++-style typedef for PIOL_Meta, ensuring C-compatibility.
typedef PIOL_Meta Meta;
}  // namespace PIOL
#endif  // __cplusplus

/*! The available trace parameters
 */

/// Copy all headers. Don't interpret them.
#define PIOL_META_COPY ((PIOL_Meta)0)

/// Local trace number.
#define PIOL_META_ltn ((PIOL_Meta)1)

/// Global trace number.
#define PIOL_META_gtn ((PIOL_Meta)2)

/// The trace number (line)
#define PIOL_META_tnl ((PIOL_Meta)3)

/// The Trace number (record)
#define PIOL_META_tnr ((PIOL_Meta)4)

/// The trace number (file)
#define PIOL_META_tn ((PIOL_Meta)5)

/// The trace number (ensemble)
#define PIOL_META_tne ((PIOL_Meta)6)

/// Number of samples in this trace
#define PIOL_META_ns ((PIOL_Meta)7)

/// The increment of this trace.
#define PIOL_META_inc ((PIOL_Meta)8)

/// Trace identification code
#define PIOL_META_Tic ((PIOL_Meta)9)

/// Source Number
#define PIOL_META_SrcNum ((PIOL_Meta)10)

/// Shot number
#define PIOL_META_ShotNum ((PIOL_Meta)11)

/// Number of traces stacked for this trace (vertical)
#define PIOL_META_VStack ((PIOL_Meta)12)

/// Number of traces stacked for this trace (horizontal)
#define PIOL_META_HStack ((PIOL_Meta)13)

/// Distance from source to receiver
#define PIOL_META_Offset ((PIOL_Meta)14)

/// Receiver group elevation
#define PIOL_META_RGElev ((PIOL_Meta)15)

/// Source surface elevation
#define PIOL_META_SSElev ((PIOL_Meta)16)

/// Source depth
#define PIOL_META_SDElev ((PIOL_Meta)17)

/// The Water depth at source
#define PIOL_META_WtrDepSrc ((PIOL_Meta)18)

/// The Water depth at receiver
#define PIOL_META_WtrDepRcv ((PIOL_Meta)19)

/// The source x coordinate
#define PIOL_META_xSrc ((PIOL_Meta)20)

/// The source y coordinate
#define PIOL_META_ySrc ((PIOL_Meta)21)

/// The receiver x coordinate
#define PIOL_META_xRcv ((PIOL_Meta)22)

/// The receiver y coordinate
#define PIOL_META_yRcv ((PIOL_Meta)23)

/// The CMP x coordinate
#define PIOL_META_xCmp ((PIOL_Meta)24)

/// The CMP y coordinate
#define PIOL_META_yCmp ((PIOL_Meta)25)

/// The inline number
#define PIOL_META_il ((PIOL_Meta)26)

/// The crossline number
#define PIOL_META_xl ((PIOL_Meta)27)

/// Unit system for transduction constant
#define PIOL_META_TransUnit ((PIOL_Meta)28)

/// Unit system for traces
#define PIOL_META_TraceUnit ((PIOL_Meta)29)

// Non-standard

/// The sum of the differences between sources and receivers of this trace and
/// another
#define PIOL_META_dsdr ((PIOL_Meta)30)

// TODO: Don't add more of these, find out what they do and replace them with
//       real names

/// Miscellaneous
#define PIOL_META_Misc1 ((PIOL_Meta)31)

/// Miscellaneous
#define PIOL_META_Misc2 ((PIOL_Meta)32)

/// Miscellaneous
#define PIOL_META_Misc3 ((PIOL_Meta)33)

/// Miscellaneous
#define PIOL_META_Misc4 ((PIOL_Meta)34)


/*! SEG-Y Trace Header offsets
 */
typedef size_t PIOL_Tr;

#ifdef __cplusplus
namespace PIOL {

/// A C++-style type for PIOL_Tr, ensuring C-compatibility
typedef PIOL_Tr Tr;

}  // namespace PIOL
#endif  // __cplusplus

/// int32_t. The trace sequence number in the Line.
#define PIOL_TR_SeqNum ((PIOL_Tr)1U)

/// int32_t. The trace sequence number in SEG-Y File.
#define PIOL_TR_SeqFNum ((PIOL_Tr)5U)

/// int32_t. The original field record number.
#define PIOL_TR_ORF ((PIOL_Tr)9U)

/// int32_t. The trace number in the ORF.
#define PIOL_TR_TORF ((PIOL_Tr)13U)

/// int32_t. The source energy number.
#define PIOL_TR_ENSrcNum ((PIOL_Tr)17U)

/// int32_t. The trace number in the ensemble.
#define PIOL_TR_SeqNumEns ((PIOL_Tr)25U)

/// int16_t. The trace identification number.
#define PIOL_TR_TIC ((PIOL_Tr)29U)

/// int16_t. The number of traces vertically stacked.
#define PIOL_TR_VStackCnt ((PIOL_Tr)31U)

/// int16_t. The number of traces horizontally stacked.
#define PIOL_TR_HStackCnt ((PIOL_Tr)33U)

/// int32_t. The distance from source center to receiver centre.
#define PIOL_TR_CDist ((PIOL_Tr)37U)

/// int32_t. The receiver group elevation.
#define PIOL_TR_RcvElv ((PIOL_Tr)41U)

/// int32_t. The surface elevation at the source.
#define PIOL_TR_SurfElvSrc ((PIOL_Tr)45U)

/// int32_t. The source depth below surface (opposite of above?).
#define PIOL_TR_SrcDpthSurf ((PIOL_Tr)49U)

/// int32_t. The datum elevation for the receiver group.
#define PIOL_TR_DtmElvRcv ((PIOL_Tr)53U)

/// int32_t. The datum elevation for the source.
#define PIOL_TR_DtmElvSrc ((PIOL_Tr)57U)

/// int32_t. The water depth for the source.
#define PIOL_TR_WtrDepSrc ((PIOL_Tr)61U)

/// int32_t. The water depth for the receive group.
#define PIOL_TR_WtrDepRcv ((PIOL_Tr)65U)

/// int16_t. The scale coordinate for 41-68 (elevations + depths).
#define PIOL_TR_ScaleElev ((PIOL_Tr)69U)

/// int16_t. The scale coordinate for 73-88 + 181-188
#define PIOL_TR_ScaleCoord ((PIOL_Tr)71U)

/// int32_t. The X coordinate for the source
#define PIOL_TR_xSrc ((PIOL_Tr)73U)

/// int32_t. The Y coordinate for the source
#define PIOL_TR_ySrc ((PIOL_Tr)77U)

/// int32_t. The X coordinate for the receive group
#define PIOL_TR_xRcv ((PIOL_Tr)81U)

/// int32_t. The Y coordinate for the receive group
#define PIOL_TR_yRcv ((PIOL_Tr)85U)

/// int16_t. The uphole time at the source (ms).
#define PIOL_TR_UpSrc ((PIOL_Tr)95U)

/// int16_t. The uphole time at the receive group (ms).
#define PIOL_TR_UpRcv ((PIOL_Tr)97U)

/// int16_t. The number of samples in the trace.
#define PIOL_TR_Ns ((PIOL_Tr)115U)

/// int16_t. The sample interval (us).
#define PIOL_TR_Inc ((PIOL_Tr)117U)

/// int32_t. The X coordinate for the CMP
#define PIOL_TR_xCmp ((PIOL_Tr)181U)

/// int32_t. The Y coordinate for the CMP
#define PIOL_TR_yCmp ((PIOL_Tr)185U)

/// int32_t. The Inline grid point.
#define PIOL_TR_il ((PIOL_Tr)189U)

/// int32_t. The Crossline grid point.
#define PIOL_TR_xl ((PIOL_Tr)193U)

/// int32_t. The source nearest to the CDP.
#define PIOL_TR_ShotNum ((PIOL_Tr)197U)

/// int16_t. The shot number scalar. (Explicitly says that 0 == 1)
#define PIOL_TR_ShotScal ((PIOL_Tr)201U)

/// int16_t. The unit system used for trace values.
#define PIOL_TR_ValMeas ((PIOL_Tr)203U)

/// int32_t. The transduction constant.
#define PIOL_TR_TransConst ((PIOL_Tr)205U)

/// int16_t. The transduction exponent.
#define PIOL_TR_TransExp ((PIOL_Tr)209U)

/// int16_t. The transduction units
#define PIOL_TR_TransUnit ((PIOL_Tr)211U)

/// int16_t. Scalar for time measurements.
#define PIOL_TR_TimeScal ((PIOL_Tr)215U)

/// int32_t. Source measurement.
#define PIOL_TR_SrcMeas ((PIOL_Tr)225U)

/// int16_t. Source measurement exponent.
#define PIOL_TR_SrcMeasExp ((PIOL_Tr)229U)


/*! An enum class of the different types of sorting operation.
 */
typedef size_t PIOL_SortType;

#ifdef __cplusplus
namespace PIOL {

/// A C++-style typedef for PIOL_SortType, ensuring C-compatibility
typedef PIOL_SortType SortType;

}  // namespace PIOL
#endif  // __cplusplus

/// Sort by source x, source y, receiver x, receiver y
#define PIOL_SORTTYPE_SrcRcv ((PIOL_SortType)35)

/// Sort by source x, source y, calcuated offset
#define PIOL_SORTTYPE_SrcOff ((PIOL_SortType)36)

/// Sort by source x, source y, offset read from file
#define PIOL_SORTTYPE_SrcROff ((PIOL_SortType)37)

/// Sort by receiver x, receiver y, calculate offset
#define PIOL_SORTTYPE_RcvOff ((PIOL_SortType)38)

/// Sort by receiver x, receiver y, offset read from file
#define PIOL_SORTTYPE_RcvROff ((PIOL_SortType)39)

/// Sort by inline, crossline, calculated offset
#define PIOL_SORTTYPE_LineOff ((PIOL_SortType)40)

/// Sort by inline, crossline, offset read from file
#define PIOL_SORTTYPE_LineROff ((PIOL_SortType)41)

/// Sort by calculated offset, inline, crossline
#define PIOL_SORTTYPE_OffLine ((PIOL_SortType)42)

/// Sort by offset read from file, inline, crossline
#define PIOL_SORTTYPE_ROffLine ((PIOL_SortType)43)


/*! An enum class of the different types of tapering windows.
 */
typedef size_t PIOL_TaperType;

#ifdef __cplusplus
namespace PIOL {

/// A C++-style typedef of PIOL_TaperType, ensuring C-compatibility
typedef PIOL_TaperType TaperType;

}  // namespace PIOL
#endif  // __cplusplus

/// Taper using a linear ramp
#define PIOL_TAPERTYPE_Linear ((PIOL_TaperType)44)

/// Taper using a cos ramp
#define PIOL_TAPERTYPE_Cos ((PIOL_TaperType)45)

/// Taper using a cos^2 ramp
#define PIOL_TAPERTYPE_CosSqr ((PIOL_TaperType)46)


/*! An enum class of the different types of automatic gain control functions.
 */
typedef size_t PIOL_AGCType;
#ifdef __cplusplus
namespace PIOL {

/// A C++-style typedef of PIOL_AGCType, ensuring C-compatibility
typedef PIOL_AGCType AGCType;

}  // namespace PIOL
#endif  // __cplusplus

/// AGC using RMS with rectangular window
#define PIOL_AGCTYPE_RMS ((PIOL_AGCType)47)

/// AGC using RMS with triangular window
#define PIOL_AGCTYPE_RMSTri ((PIOL_AGCType)48)

/// AGC using mean absolute value with rectangular window
#define PIOL_AGCTYPE_MeanAbs ((PIOL_AGCType)49)

/// AGC using the median value
#define PIOL_AGCTYPE_Median ((PIOL_AGCType)50)


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


/*! A structure to hold a reference to a single coordinate and the corresponding
 *  trace number.
 *  Defined as struct for C-compatibility.
 */
struct PIOL_CoordElem {
    /// The value
    PIOL_geom_t val;

    /// The trace number
    size_t num;
};

#ifdef __cplusplus
namespace PIOL {

/// A C++-style typedef of PIOL_CoordElem, ensuring C-compatibility
typedef PIOL_CoordElem CoordElem;

}  // namespace PIOL
#endif  // __cplusplus


#ifdef __cplusplus
namespace PIOL {
namespace File {

struct Param;

}  // namespace File
}  // namespace PIOL

/// Typedef of C-compatible typename for opaque pointer to PIOL::File::Param
typedef PIOL::File::Param PIOL_Param;
#else
/// Typedef of C-compatible typename for opaque pointer to PIOL::File::Param
typedef struct PIOL_Param PIOL_Param;
#endif

/// The NULL parameter so that the correct internal read pattern is selected
#define PIOL_PARAM_NULL ((PIOL_Param*)1)

#endif
