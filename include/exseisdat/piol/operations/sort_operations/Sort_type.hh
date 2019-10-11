////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_OPERATIONS_SORT_OPERATIONS_SORT_TYPE_HH
#define EXSEISDAT_PIOL_OPERATIONS_SORT_OPERATIONS_SORT_TYPE_HH

#include <cstddef>

namespace exseis {
namespace piol {
inline namespace operations {
inline namespace sort_operations {

/// @brief An enumeration of the different types of sorting operation.
enum class Sort_type : size_t {
    /// Sort by source x, source y, receiver x, receiver y
    SrcRcv,

    /// Sort by source x, source y, calcuated offset
    SrcOff,

    /// Sort by source x, source y, offset read from file
    SrcROff,

    /// Sort by receiver x, receiver y, calculate offset
    RcvOff,

    /// Sort by receiver x, receiver y, offset read from file
    RcvROff,

    /// Sort by inline, crossline, calculated offset
    LineOff,

    /// Sort by inline, crossline, offset read from file
    LineROff,

    /// Sort by calculated offset, inline, crossline
    OffLine,

    /// Sort by offset read from file, inline, crossline
    ROffLine
};

}  // namespace sort_operations
}  // namespace operations
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_OPERATIONS_SORT_OPERATIONS_SORT_TYPE_HH
