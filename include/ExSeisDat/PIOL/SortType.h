////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_SORTTYPE_H
#define EXSEISDAT_PIOL_SORTTYPE_H

#include <stddef.h>

/*! An enum class of the different types of sorting operation.
 */
typedef size_t PIOL_SortType;

#ifdef __cplusplus
namespace exseis {
namespace PIOL {
/// @copydoc PIOL_SortType
typedef PIOL_SortType SortType;
}  // namespace PIOL
}  // namespace exseis
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

#endif  // EXSEISDAT_PIOL_SORTTYPE_H
