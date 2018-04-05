////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Typedefs used extensively throughout ExSeisDat.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_TYPEDEFS_H
#define EXSEISDAT_PIOL_TYPEDEFS_H

#include "stddef.h"


/// A large signed integer type.
typedef ptrdiff_t PIOL_llint;

#ifdef __cplusplus
namespace PIOL {
/// @copydoc PIOL_llint
typedef PIOL_llint llint;
}  // namespace PIOL
#endif  // __cplusplus


/// The type used to store trace values
typedef float PIOL_trace_t;

#ifdef __cplusplus
namespace PIOL {
/// @copydoc PIOL_trace_t
typedef PIOL_trace_t trace_t;
}  // namespace PIOL
#endif  // __cplusplus


/// The type used for manipulating geometry and physical values
typedef double PIOL_geom_t;

#ifdef __cplusplus
namespace PIOL {
/// @copydoc PIOL_geom_t
typedef PIOL_geom_t geom_t;
}  // namespace PIOL
#endif  // __cplusplus


#ifdef __cplusplus

#include <complex>

namespace PIOL {

/// The type to use for accessing individual bytes
typedef unsigned char uchar;

/// Real type for physical, SI and math units
typedef double unit_t;

/// Complex type for traces
typedef std::complex<trace_t> cmtrace_t;

}  // namespace PIOL

#endif  // __cplusplus

#endif  // EXSEISDAT_PIOL_TYPEDEFS_H
