////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Typedefs used extensively throughout ExSeisDat.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_TYPEDEFS_HH
#define EXSEISDAT_UTILS_TYPEDEFS_HH

#include <cstdint>

///
/// @namespace exseis::utils::typedefs
///
/// @brief Aliases for fundamental types used throughout the ExSeisDat project.
///

namespace exseis {
namespace utils {
inline namespace typedefs {

/// A fundamental signed integer type.
///
/// This is used where fundamental integer type is needed, large enough to hold
/// whatever value we might need it to, without explicitly specifying the size.
///
using Integer = int64_t;


/// A fundamental signed floating-point type.
///
/// This is used where fundamental floating-point type is needed, large enough
/// to hold whatever value we might need it to, without explicitly specifying
/// the size.
///
using Floating_point = double;


/// The floating-point type used to store trace values.
///
/// Traces values are usually stored in a lossy, compressed form.
/// As a result, we don't need a high-precision storage type.
///
using Trace_value = float;

}  // namespace typedefs
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_TYPEDEFS_HH
