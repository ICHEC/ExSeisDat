////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Typedef for the Taper_function type, and declarations for the
///        built-in taper functions.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_FUNCTION_H
#define EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_FUNCTION_H

#include "ExSeisDat/utils/c_api_utils.h"
#include "ExSeisDat/utils/typedefs.h"

#ifdef __cplusplus
namespace exseis {
namespace utils {
inline namespace signal_processing {
#endif  // __cplusplus

/// @name C API
/// @{

/// @copydoc Taper_function
typedef exseis_Trace_value (*exseis_Taper_function)(
  exseis_Trace_value position, exseis_Trace_value width);

/// @} C API


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
#ifdef __cplusplus
using Taper_function = Trace_value (*)(Trace_value position, Trace_value width);

static_assert(
  std::is_same<Taper_function, exseis_Taper_function>::value,
  "exseis::utils::Taper_function and exseis_Taper_function are not the same type!");
#endif  // __cplusplus


/// @name Taper Functions
/// @{

/// @brief A linear taper function.
///
/// A linear interpolation between 0 and 1.
///
/// @copydetails Taper_function
///
/// @remark C API: \ref exseis_linear_taper
///
#ifdef __cplusplus
Trace_value linear_taper(Trace_value position, Trace_value width);
#endif  // __cplusplus


/// @brief A cosine taper function
///
/// A cosine interpolation between 0 and 1.
///
/// @copydetails Taper_function
///
/// @remark C API: \ref exseis_cosine_taper
///
#ifdef __cplusplus
Trace_value cosine_taper(Trace_value position, Trace_value width);
#endif  // __cplusplus


/// @brief A cosine square taper function
///
/// A cos^2 interpolation between 0 and 1.
///
/// @copydetails Taper_function
///
/// @remark C API: \ref exseis_cosine_square_taper
///
#ifdef __cplusplus
Trace_value cosine_square_taper(Trace_value position, Trace_value width);
#endif  // __cplusplus

/// @} Taper Functions


/// @name C API
/// @{

/// @brief C API for \ref linear_taper
/// @copydoc linear_taper
EXSEISDAT_CXX_ONLY(extern "C")
exseis_Trace_value exseis_linear_taper(
  exseis_Trace_value position, exseis_Trace_value width);


/// @brief C API for \ref cosine_taper
/// @copydoc cosine_taper
EXSEISDAT_CXX_ONLY(extern "C")
exseis_Trace_value exseis_cosine_taper(
  exseis_Trace_value position, exseis_Trace_value width);


/// @brief The C API for \ref cosine_square_taper
/// @copydoc cosine_taper
EXSEISDAT_CXX_ONLY(extern "C")
exseis_Trace_value exseis_cosine_square_taper(
  exseis_Trace_value position, exseis_Trace_value width);

/// @} C API


#ifdef __cplusplus
}  // inline namespace signal_processing
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_FUNCTION_H
