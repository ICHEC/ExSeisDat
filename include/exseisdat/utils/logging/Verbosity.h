////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C API for \ref exseisdat/utils/logging/Verbosity.hh
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_LOGGING_VERBOSITY_H
#define EXSEISDAT_UTILS_LOGGING_VERBOSITY_H

#include <stddef.h>

#ifdef __cplusplus

namespace exseis {
namespace utils {
inline namespace logging {

extern "C" {
#endif  // __cplusplus

/// @name C API
///
/// @{

/// @brief C API for \ref exseis::utils::logging::Verbosity
/// @copydoc exseis::utils::logging::Verbosity
typedef size_t exseis_Verbosity;


/// @brief C API for \ref exseis::utils::logging::Verbosity
/// @copydoc exseis::utils::logging::Verbosity
enum {
    /// Record no information beyond fatal errors and explicit requests
    exseis_verbosity_none = 0,

    /// Record essential information including other errors and warnings
    exseis_verbosity_minimal = 1,

    /// Record some non-warnings
    exseis_verbosity_extended = 2,

    /// Record a high level of information
    exseis_verbosity_verbose = 3,

    /// Maximum Verbosity level. (for debugging)
    exseis_verbosity_max = 4
};

/// @} C API

#ifdef __cplusplus
}  // extern "C"

}  // namespace logging
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus


#endif  // EXSEISDAT_UTILS_LOGGING_VERBOSITY_H
