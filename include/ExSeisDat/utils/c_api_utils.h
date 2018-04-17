///
/// @file
/// @brief This file contains a number of macros for cleanly defining the C
///        and C++ APIs side by side.
///
#ifndef EXSEISDAT_UTILS_C_API_UTILS_H
#define EXSEISDAT_UTILS_C_API_UTILS_H

/// Includes the macro arguments in the file only if it's being processed by
/// a C++ compiler.
///
/// This is used for specifying C and C++ interfaces side-by-side in the same
/// file without lots of ifdef __cplusplus everywhere.
/// It's particularly clear for adding things line `extern "C"` in front of
/// functions in the C API.
#ifdef __cplusplus
#define EXSEISDAT_CXX_ONLY(X) X
#else
#define EXSEISDAT_CXX_ONLY(X)
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_C_API_UTILS_H
