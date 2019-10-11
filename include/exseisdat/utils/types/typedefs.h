////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C API for \ref exseisdat/utils/types/typedefs.hh
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_TYPES_TYPEDEFS_H
#define EXSEISDAT_UTILS_TYPES_TYPEDEFS_H

#include "stdint.h"


#ifdef __cplusplus
namespace exseis {
namespace utils {
inline namespace types {

#endif  // __cplusplus

/// @name C_API
/// @{

/// @brief C API for exseis::utils::types::Integer
/// @copydoc exseis::utils::types::Integer
typedef int64_t exseis_Integer;

/// @brief C API for exseis::utils::types::Floating_point
/// @copydoc exseis::utils::types::Floating_point
typedef double exseis_Floating_point;

/// @brief C API for exseis::utils::types::Trace_value
/// @copydoc exseis::utils::types::Trace_value
typedef float exseis_Trace_value;

/// @} C API

#ifdef __cplusplus

}  // namespace types
}  // namespace utils
}  // namespace exseis

#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_TYPES_TYPEDEFS_H
