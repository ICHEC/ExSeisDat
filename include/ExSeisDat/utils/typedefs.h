////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Typedefs used extensively throughout ExSeisDat.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_TYPEDEFS_H
#define EXSEISDAT_UTILS_TYPEDEFS_H

#include "stddef.h"

#ifdef __cplusplus
#include <type_traits>
#endif  // __cplusplus


///
/// @namespace exseis::utils::typedefs
///
/// Aliases for fundamental types used throughout the ExSeisDat project.
///


/// @copydoc exseis::utils::typedefs::Integer
typedef long long int exseis_Integer;

#ifdef __cplusplus
namespace exseis {
namespace utils {
inline namespace typedefs {

/// A fundamental signed integer type.
///
/// This is used where fundamental integer type is needed, large enough to hold
/// whatever value we might need it to, without explicitly specifying the size.
///
using Integer = long long int;

static_assert(
  std::is_same<Integer, exseis_Integer>::value,
  "exseis::utils::Integer and exseis_Integer are not the same type!");

}  // namespace typedefs
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus


/// @copydoc exseis::utils::typedefs::Floating_point
typedef double exseis_Floating_point;

#ifdef __cplusplus
namespace exseis {
namespace utils {
inline namespace typedefs {

/// A fundamental signed floating-point type.
///
/// This is used where fundamental floating-point type is needed, large enough
/// to hold whatever value we might need it to, without explicitly specifying
/// the size.
///
using Floating_point = double;

static_assert(
  std::is_same<Floating_point, exseis_Floating_point>::value,
  "exseis::utils::Floating_point and exseis_Floating_point are not the same type!");

}  // namespace typedefs
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus


/// @copydoc exseis::utils::typedefs::Trace_value
typedef float exseis_Trace_value;

#ifdef __cplusplus
namespace exseis {
namespace utils {
inline namespace typedefs {

/// The floating-point type used to store trace values.
///
/// Traces values are usually stored in a lossy, compressed form.
/// As a result, we don't need a high-precision storage type.
///
using Trace_value = float;

static_assert(
  std::is_same<Trace_value, exseis_Trace_value>::value,
  "exseis::utils::Trace_value and exseis_Trace_value are not the same type!");

}  // namespace typedefs
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_TYPEDEFS_H
