////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C API for \ref exseisdat/piol/operations/sort_operations/Sort_type.hh
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_OPERATIONS_SORT_OPERATIONS_SORTTYPE_H
#define EXSEISDAT_PIOL_OPERATIONS_SORT_OPERATIONS_SORTTYPE_H

#include <stddef.h>

#ifdef __cplusplus
namespace exseis {
namespace piol {
inline namespace operations {
inline namespace sort_operations {

extern "C" {
#endif  // __cplusplus

/// @name C API
///
/// @{

/// @brief C API for exseis::piol::operations::sort_operations::Sort_type
/// @copydoc exseis::piol::operations::sort_operations::Sort_type
typedef size_t exseis_SortType;

/// @brief C API for exseis::piol::operations::sort_operations::Sort_type
/// @copydoc exseis::piol::operations::sort_operations::Sort_type
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

/// @} C API

#ifdef __cplusplus
}  // extern "C"

}  // namespace sort_operations
}  // namespace operations
}  // namespace piol
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_PIOL_OPERATIONS_SORT_OPERATIONS_SORTTYPE_H
