////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief
/// @details Insert trivial typdefs etc into this file which will be used
///          throughout the code-base.
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLGLOBAL_INCLUDE_GUARD
#define PIOLGLOBAL_INCLUDE_GUARD

#include "stddef.h"

#ifdef __cplusplus
#include <complex>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#endif  // __cplusplus


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// A pointer diff type
typedef ptrdiff_t PIOL_llint;

/// The type used to store trace values
typedef float PIOL_trace_t;

/// The type used for manipulating geometry and physical values
typedef double PIOL_geom_t;

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus


#ifdef __cplusplus

namespace PIOL {

/// The type to use for accessing individual bytes
typedef unsigned char uchar;

/// A large signed integer type.
typedef PIOL_llint llint;

/// Real type for traces, use "*_t" convention
typedef PIOL_trace_t trace_t;

/// Real type for geometry
typedef PIOL_geom_t geom_t;

/// Real type for physical, SI and math units
typedef double unit_t;

/// Real type for traces, use "*_t" convention
typedef std::complex<trace_t> cmtrace_t;


/// A typedef for the agc stats function
typedef std::function<trace_t(trace_t*, size_t, trace_t, llint)> AGCFunc;

/// A typedef for the agc stats function
typedef std::function<trace_t(trace_t, trace_t)> TaperFunc;

/// Typedef for filter padding funcitons
typedef std::function<trace_t(trace_t*, size_t, size_t, size_t)> FltrPad;

/// Return the value associated with a particular parameter
template<typename T>
using MinMaxFunc = std::function<geom_t(const T&)>;

/// A template for the Compare less-than function
template<class T>
using Compare = std::function<bool(const T&, const T&)>;

namespace Data {
class Interface;
}  // namespace Data

namespace Obj {
class Interface;
}  // namespace Obj

namespace File {
class ReadInterface;
class WriteInterface;
}  // namespace File

}  // namespace PIOL

#endif  // __cplusplus

#endif  // PIOLGLOBAL_INCLUDE_GUARD
