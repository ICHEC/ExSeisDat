////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C API for \ref exseisdat/utils/signal_processing/Taper_function.hh
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_FUNCTION_H
#define EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_FUNCTION_H

#include "exseisdat/utils/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// @name C API
/// @{

/// @brief C API for \ref exseis::utils::signal_processing::Taper_function
/// @copydoc exseis::utils::signal_processing::Taper_function
typedef exseis_Trace_value (*exseis_Taper_function)(
  exseis_Trace_value position, exseis_Trace_value width);


/// @brief C API for \ref exseis::utils::signal_processing::linear_taper
/// @copydoc exseis::utils::signal_processing::linear_taper
exseis_Trace_value exseis_linear_taper(
  exseis_Trace_value position, exseis_Trace_value width);


/// @brief C API for \ref exseis::utils::signal_processing::cosine_taper
/// @copydoc exseis::utils::signal_processing::cosine_taper
exseis_Trace_value exseis_cosine_taper(
  exseis_Trace_value position, exseis_Trace_value width);


/// @brief The C API for
///        \ref exseis::utils::signal_processing::cosine_square_taper
/// @copydoc exseis::utils::signal_processing::cosine_square_taper
exseis_Trace_value exseis_cosine_square_taper(
  exseis_Trace_value position, exseis_Trace_value width);

/// @} C API


#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_FUNCTION_H
