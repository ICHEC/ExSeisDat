/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date December 2016
 *//*******************************************************************************************/
#ifndef PIOLSHAREAPI_INCLUDE_GUARD
#define PIOLSHAREAPI_INCLUDE_GUARD

#ifndef __cplusplus
typedef double geom_t;
#endif

#ifdef __cplusplus
namespace PIOL {
/*! The available trace parameters
 */
enum class Meta : int
#else
typedef enum
#endif
{
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
    CDist,      //!< Distance from source centre to receiver centre.
    RGElev,     //!< Receiver group elevation
    SSElev,     //!< Source surface elevation
    SDElev,     //!< Source depth
    xSrc,       //!< The source x coordiante
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

/*! An enum class of the different types of sorting operaton.
 */
#ifdef __cplusplus
enum class SortType : int
#else
typedef enum
#endif
{
    SrcRcv,     //!< Sort by source x, source y, receiver x, receiver y
    SrcOff,
    RcvOff,
    LineOff,
    OffLine
#ifdef __cplusplus
};
#else
} SortType;
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
