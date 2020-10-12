////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Verbosity levels for Logging
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_UTILS_LOGGING_VERBOSITY_HH
#define EXSEIS_UTILS_LOGGING_VERBOSITY_HH

#include <cstddef>

namespace exseis {
inline namespace utils {
inline namespace logging {


/// @brief Enumeration of the verbosity levels for logging.
///
///
enum class Verbosity : size_t {
    /// Record no information beyond fatal errors and explicit requests
    none = 0,

    /// Record essential information including other errors and warnings
    minimal = 1,

    /// Record some non-warnings
    extended = 2,

    /// Record a high level of information
    verbose = 3,

    /// Maximum Verbosity level. (for debugging)
    max = 4
};


}  // namespace logging
}  // namespace utils
}  // namespace exseis

#endif  // EXSEIS_UTILS_LOGGING_VERBOSITY_HH
