////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Typedefs used extensively throughout ExSeisDat.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_TYPES_TYPEDEFS_HH
#define EXSEISDAT_UTILS_TYPES_TYPEDEFS_HH

#include <cstdint>

namespace exseis {
namespace utils {
inline namespace types {

/// @brief A fundamental signed integer type.
///
/// @details This is used where fundamental integer type is needed, large enough
///          to hold whatever value we might need it to, without explicitly
///          specifying the size.
///
using Integer = int64_t;


/// @brief A fundamental signed floating-point type.
///
/// @details This is used where fundamental floating-point type is needed, large
///          enough to hold whatever value we might need it to, without
///          explicitly specifying the size.
///
using Floating_point = double;


/// @brief The floating-point type used to store trace values.
///
/// @details Traces values are usually stored in a lossy, compressed form.
///          As a result, we don't need a high-precision storage type.
///
using Trace_value = float;

}  // namespace types
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_TYPES_TYPEDEFS_HH
