////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The Source_position object and related funtions, representing a
///        position in the source code, primarily for logging.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_UTILS_LOGGING_SOURCE_POSITION_HH
#define EXSEIS_UTILS_LOGGING_SOURCE_POSITION_HH

#include <sstream>
#include <string>


/// @brief Returns a formatted string of the FUNCTION_NAME in the current
///        file at the current line number.
///
/// @param[in] FUNCTION_NAME A string: The name of the current function.
///
/// @return The formatted source position this macro was called from.
///
#define EXSEIS_SOURCE_POSITION(FUNCTION_NAME)                                  \
    (::exseis::utils::logging::Source_position{(FUNCTION_NAME), __FILE__,      \
                                               __LINE__})

namespace exseis {
inline namespace utils {
inline namespace logging {


/// @brief A representation of a point in the source code.
///
/// This is intended to be used for logging the position a Log_entry was
/// created.
///
/// The macro \ref EXSEIS_SOURCE_POSITION is provided to construct this
/// from the function name with the file and line number information added
/// automatically.
///
struct Source_position {
    /// The name of the function containing the given point in the code.
    std::string function_name;

    /// The name of the file containing the given point in the code.
    std::string file_name;

    /// The line number in the file containing the given point in the code.
    size_t line_number;
};


}  // namespace logging
}  // namespace utils
}  // namespace exseis

#endif  // EXSEIS_UTILS_LOGGING_SOURCE_POSITION_HH
