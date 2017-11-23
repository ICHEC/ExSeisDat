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

typedef size_t PIOL_Meta;

#ifdef __cplusplus
namespace PIOL {
/*! The available trace parameters
 */
    typedef PIOL_Meta Meta;
}
#endif

#define PIOL_META_COPY      ((PIOL_Meta) 0)       //!< Copy all headers. Don't interpret them.
#define PIOL_META_ltn       ((PIOL_Meta) 1)        //!< Local trace number.
#define PIOL_META_gtn       ((PIOL_Meta) 2)        //!< Global trace number.
#define PIOL_META_tnl       ((PIOL_Meta) 3)        //!< The trace number (line)
#define PIOL_META_tnr       ((PIOL_Meta) 4)        //!< The Trace number (record)
#define PIOL_META_tn        ((PIOL_Meta) 5)         //!< The trace number (file)
#define PIOL_META_tne       ((PIOL_Meta) 6)        //!< The trace number (ensemble)
#define PIOL_META_ns        ((PIOL_Meta) 7)         //!< Number of samples in this trace
#define PIOL_META_inc       ((PIOL_Meta) 8)        //!< The increment of this trace.
#define PIOL_META_Tic       ((PIOL_Meta) 9)        //!< Trace identification code
#define PIOL_META_SrcNum    ((PIOL_Meta) 10)     //!< Source Number
#define PIOL_META_ShotNum   ((PIOL_Meta) 11)    //!< Shot number
#define PIOL_META_VStack    ((PIOL_Meta) 12)     //!< Number of traces stacked for this trace (vertical)
#define PIOL_META_HStack    ((PIOL_Meta) 13)     //!< Number of traces stacked for this trace (horizontal)
#define PIOL_META_Offset    ((PIOL_Meta) 14)     //!< Distance from source to receiver
#define PIOL_META_RGElev    ((PIOL_Meta) 15)     //!< Receiver group elevation
#define PIOL_META_SSElev    ((PIOL_Meta) 16)     //!< Source surface elevation
#define PIOL_META_SDElev    ((PIOL_Meta) 17)     //!< Source depth
#define PIOL_META_WtrDepSrc ((PIOL_Meta) 18)  //!< The Water depth at source
#define PIOL_META_WtrDepRcv ((PIOL_Meta) 19)  //!< The Water depth at receiver
#define PIOL_META_xSrc      ((PIOL_Meta) 20)       //!< The source x coordinate
#define PIOL_META_ySrc      ((PIOL_Meta) 21)       //!< The source y coordinate
#define PIOL_META_xRcv      ((PIOL_Meta) 22)       //!< The receiver x coordinate
#define PIOL_META_yRcv      ((PIOL_Meta) 23)       //!< The receiver y coordinate
#define PIOL_META_xCmp      ((PIOL_Meta) 24)       //!< The CMP x coordinate
#define PIOL_META_yCmp      ((PIOL_Meta) 25)       //!< The CMP y coordinate
#define PIOL_META_il        ((PIOL_Meta) 26)         //!< The inline number
#define PIOL_META_xl        ((PIOL_Meta) 27)         //!< The crossline number
#define PIOL_META_TransUnit ((PIOL_Meta) 28)  //!< Unit system for transduction constant
#define PIOL_META_TraceUnit ((PIOL_Meta) 29)  //!< Unit system for traces
// Non-standard
#define PIOL_META_dsdr      ((PIOL_Meta) 30)       //!< The sum of the differences between sources and receivers of this trace and another
//TODO: Don't add more of th((PIOL_Meta)se, find out what they do and replace them with real names
#define PIOL_META_Misc1     ((PIOL_Meta) 31)      //!< Miscellaneous
#define PIOL_META_Misc2     ((PIOL_Meta) 32)      //!< Miscellaneous
#define PIOL_META_Misc3     ((PIOL_Meta) 33)      //!< Miscellaneous
#define PIOL_META_Misc4     ((PIOL_Meta) 34)      //!< Miscellaneous


/*! An enum class of the different types of sorting operation.
 */
#ifdef __cplusplus
namespace PIOL {

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
