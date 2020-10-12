////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The Status enum, representing the status of a log message.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_UTILS_LOGGING_STATUS_HH
#define EXSEIS_UTILS_LOGGING_STATUS_HH

#include <string>


namespace exseis {
inline namespace utils {
inline namespace logging {


/// @brief The enumeration for the different status messages.
enum class Status : size_t {
    /// A general note item.
    Note,

    /// A debug item
    Debug,

    /// A warning item
    Warning,

    /// A serious error which should lead to program termination
    Error,

    /// A message the end-user has specifically requested.
    Request
};


/// @brief Get a human readable string for the given status.
///
/// @param[in] status The status to get a string for.
///
/// @return A human readable string representing the status.
///
std::string status_to_string(Status status);


}  // namespace logging
}  // namespace utils
}  // namespace exseis

#endif  // EXSEIS_UTILS_LOGGING_STATUS_HH
