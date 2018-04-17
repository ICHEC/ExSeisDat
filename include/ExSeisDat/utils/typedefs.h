////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Typedefs used extensively throughout ExSeisDat.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_TYPEDEFS_H
#define EXSEISDAT_UTILS_TYPEDEFS_H

#include "stddef.h"


/// A large signed integer type.
typedef ptrdiff_t exseis_llint;

#ifdef __cplusplus
namespace exseis {
namespace utils {
inline namespace typedefs {
/// @copydoc exseis_llint
using llint = exseis_llint;
}  // namespace typedefs
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus


/// The type used to store trace values
typedef float exseis_trace_t;

#ifdef __cplusplus
namespace exseis {
namespace utils {
inline namespace typedefs {
/// @copydoc exseis_trace_t
using trace_t = exseis_trace_t;
}  // namespace typedefs
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus


/// The type used for manipulating geometry and physical values
typedef double exseis_geom_t;

#ifdef __cplusplus
namespace exseis {
namespace utils {
inline namespace typedefs {
/// @copydoc exseis_geom_t
using geom_t = exseis_geom_t;
}  // namespace typedefs
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus


#ifdef __cplusplus

#include <complex>

namespace exseis {
namespace utils {
inline namespace typedefs {

/// The type to use for accessing individual bytes
using uchar = unsigned char;

/// Real type for physical, SI and math units
using unit_t = double;

/// Complex type for traces
using cmtrace_t = std::complex<trace_t>;

}  // namespace typedefs
}  // namespace utils
}  // namespace exseis

#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_TYPEDEFS_H
