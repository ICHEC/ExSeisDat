////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C API for \ref exseisdat/piol/operations/SortType.hh
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_OPERATIONS_SORTTYPE_H
#define EXSEISDAT_PIOL_OPERATIONS_SORTTYPE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// @brief C API for exseis::piol::SortType
/// @copydoc exseis::piol::SortType
typedef size_t exseis_SortType;

/// @brief C API for exseis::piol::SortType
/// @copydoc exseis::piol::SortType
enum {
    /// Sort by source x, source y, receiver x, receiver y
    exseis_sorttype_SrcRcv,

    /// Sort by source x, source y, calcuated offset
    exseis_sorttype_SrcOff,

    /// Sort by source x, source y, offset read from file
    exseis_sorttype_SrcROff,

    /// Sort by receiver x, receiver y, calculate offset
    exseis_sorttype_RcvOff,

    /// Sort by receiver x, receiver y, offset read from file
    exseis_sorttype_RcvROff,

    /// Sort by inline, crossline, calculated offset
    exseis_sorttype_LineOff,

    /// Sort by inline, crossline, offset read from file
    exseis_sorttype_LineROff,

    /// Sort by calculated offset, inline, crossline
    exseis_sorttype_OffLine,

    /// Sort by offset read from file, inline, crossline
    exseis_sorttype_ROffLine
};


#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // EXSEISDAT_PIOL_OPERATIONS_SORTTYPE_H
