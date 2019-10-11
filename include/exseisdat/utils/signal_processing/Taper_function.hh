////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Typedef for the Taper_function type, and declarations for the
///        built-in taper functions.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_FUNCTION_HH
#define EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_FUNCTION_HH

#include "exseisdat/utils/types/typedefs.hh"

#include <type_traits>

namespace exseis {
namespace utils {
inline namespace signal_processing {


/// @brief A function for tapering a signal at one or both ends.
///
/// A taper function should be 0 when `position` is 0, and 1 when
/// `position == width`.
///
/// @param[in] position The current position of the taper
///                     (between 0 and `width`).
/// @param[in] width    The total width of the taper.
///
/// @returns The taper scaling, in range [0, 1].
///
/// @pre position > 0
/// @pre width > 0
/// @pre position <= width
///
using Taper_function = Trace_value (*)(Trace_value position, Trace_value width);


/// @name Taper Functions
/// @{

/// @brief A linear taper function.
///
/// A linear interpolation between 0 and 1.
///
/// @copydetails exseis::utils::signal_processing::Taper_function
///
/// @remark C API: \ref exseis_linear_taper
///
Trace_value linear_taper(Trace_value position, Trace_value width);


/// @brief A cosine taper function
///
/// A cosine interpolation between 0 and 1.
///
/// @copydetails exseis::utils::signal_processing::Taper_function
///
/// @remark C API: \ref exseis_cosine_taper
///
Trace_value cosine_taper(Trace_value position, Trace_value width);


/// @brief A cosine square taper function
///
/// A cos^2 interpolation between 0 and 1.
///
/// @copydetails exseis::utils::signal_processing::Taper_function
///
/// @remark C API: \ref exseis_cosine_square_taper
///
Trace_value cosine_square_taper(Trace_value position, Trace_value width);

/// @} Taper Functions

}  // namespace signal_processing
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_FUNCTION_HH
