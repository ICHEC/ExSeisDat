/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date December 2016
 *   \details This file will be called by C++ and C11 code as well as internal code and external
 *            facing library headers.
 *//*******************************************************************************************/
#ifndef PIOLSHAREAPI_INCLUDE_GUARD
#define PIOLSHAREAPI_INCLUDE_GUARD

typedef double geom_t;  //!< TODO: This should be done somewhere more general

#ifdef __cplusplus
namespace PIOL {
/*! The available trace parameters
 */
enum class Meta : int
#else
/*! The available trace parameters for C
 */
typedef enum
#endif
{
    Copy,       //!< Copy all headers. Don't interpret them.
    ltn,        //!< Local trace number.
    gtn,        //!< Global trace number.
    tnl,        //!< The trace number (line)
    tnr,        //!< The Trace number (record)
    tn,         //!< The trace number (file)
    tne,        //!< The trace number (ensemble)
    ns,         //!< Number of samples in this trace
    inc,        //!< The increment of this trace.
    Tic,        //!< Trace identification code
    SrcNum,     //!< Source Number
    ShotNum,    //!< Shot number
    VStack,     //!< Number of traces stacked for this trace (vertical)
    HStack,     //!< Number of traces stacked for this trace (horizontal)
    Offset,     //!< Distance from source to receiver
    RGElev,     //!< Receiver group elevation
    SSElev,     //!< Source surface elevation
    SDElev,     //!< Source depth
    WtrDepSrc,  //!< The Water depth at source
    WtrDepRcv,  //!< The Water depth at receiver
    xSrc,       //!< The source x coordinate
    ySrc,       //!< The source y coordinate
    xRcv,       //!< The receiver x coordinate
    yRcv,       //!< The receiver y coordinate
    xCmp,       //!< The CMP x coordinate
    yCmp,       //!< The CMP y coordinate
    il,         //!< The inline number
    xl,         //!< The crossline number
    TransUnit,  //!< Unit system for transduction constant
    TraceUnit,  //!< Unit system for traces
//Non-standard
    dsdr,       //!< The sum of the differences between sources and receivers of this trace and another
    //TODO: Don't add more of these, find out what they do and replace them with real names
    Misc1,      //!< Miscellaneous
    Misc2,      //!< Miscellaneous
    Misc3,      //!< Miscellaneous
    Misc4,      //!< Miscellaneous
#ifdef __cplusplus
};
#else
} Meta;
#endif

/*! An enum class of the different types of sorting operation.
 */
#ifdef __cplusplus
enum class SortType : int
#else
typedef enum
#endif
{
    SrcRcv,     //!< Sort by source x, source y, receiver x, receiver y
    SrcOff,     //!< Sort by source x, source y, calcuated offset
    SrcROff,    //!< Sort by source x, source y, offset read from file
    RcvOff,     //!< Sort by receiver x, receiver y, calculate offset
    RcvROff,    //!< Sort by receiver x, receiver y, offset read from file
    LineOff,    //!< Sort by inline, crossline, calculated offset
    LineROff,   //!< Sort by inline, crossline, offset read from file
    OffLine,    //!< Sort by calculated offset, inline, crossline
    ROffLine    //!< Sort by offset read from file, inline, crossline

#ifdef __cplusplus
};
#else
} SortType;
#endif

/*! An enum class of the different types of tapering windows.
 */
#ifdef __cplusplus
enum class TaperType : int
#else
typedef enum
#endif
{
    Linear,     //!< Taper using a linear ramp
    Cos,        //!< Taper using a cos ramp
    CosSqr      //!< Taper using a cos^2 ramp
#ifdef __cplusplus
};
#else
} TaperType;
#endif
/*! An enum class of the different types of filters.
 */
#ifdef __cplusplus
enum class FltrType : int
#else
typedef enum
#endif
{
    Lowpass,    //!< Create Lowpass IIR Butterworth filter
    Highpass,   //!< Create Highpass IIR Butterworth filter
    Bandpass,   //!< Create Bandpass IIR Butterworth filter
    Bandstop    //!< Create Bandstop IIR Butterworth filter
#ifdef __cplusplus
};
#else
} FltrType;
#endif

/*! An enum class of the different types of filtering domains.
 */
#ifdef __cplusplus
enum class FltrDmn : int
#else
typedef enum
#endif
{
    Time,    //!< Filter in time domain
    Freq     //!< Filter in frequency domain
#ifdef __cplusplus
};
#else
} FltrDmn;
#endif

/*! An enum class of the different types of trace padding functions.
 */
#ifdef __cplusplus
enum class PadType : int
#else
typedef enum
#endif
{
    Zero,       //!< Pad using zeros
    Symmetric,  //!< Pad using reflection of trace
    Replicate,  //!< Pad using closest value in trace
    Cyclic      //!< Pad using values from other end of trace
#ifdef __cplusplus
};
#else
} PadType;
#endif

/*! An enum class of the different types of automatic gain control functions.
 */
#ifdef __cplusplus
enum class AGCType : int
#else
typedef enum
#endif
{
    RMS,       //!< AGC using RMS with rectangular window
    RMSTri,    //!< AGC using RMS with triangular window
    MeanAbs,   //!< AGC using mean absolute value with rectangular window
    Median     //!< AGC using the median value
#ifdef __cplusplus
};
#else
} AGCType;
#endif

/*! A structure to hold a reference to a single coordinate and
 * the corresponding trace number
 */
#ifdef __cplusplus
struct CoordElem
#else
typedef struct
#endif
{
    geom_t val;     //!< The value
    size_t num;     //!< The trace number
#ifdef __cplusplus
};
#else
} CoordElem;
#endif
#ifdef __cplusplus
}
#endif
#endif
