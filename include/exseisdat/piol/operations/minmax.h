////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C API for \ref exseisdat/piol/operations/minmax.hh
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_OPERATIONS_MINMAX_H
#define EXSEISDAT_PIOL_OPERATIONS_MINMAX_H

#include "exseisdat/utils/types/typedefs.h"
#include "stddef.h"

#ifdef __cplusplus
namespace exseis {
namespace piol {
inline namespace operations {

using exseis::utils::types::exseis_Floating_point;

extern "C" {
#endif  // __cplusplus

/// @name C API
///
/// @{

/*! @brief A structure to hold a reference to a single coordinate and the
 *          corresponding trace number.
 *          Defined as struct for C-compatibility.
 */
struct PIOL_CoordElem {
    /// The value
    exseis_Floating_point val;

    /// The trace number
    size_t num;
};

/// @} C API

#ifdef __cplusplus
}  // extern "C"

}  // namespace operations
}  // namespace piol
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_PIOL_OPERATIONS_MINMAX_H
