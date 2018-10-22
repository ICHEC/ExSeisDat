////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The interface for signal scaling functions for gain control, along
///        with a number of default implementations.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_SIGNAL_PROCESSING_GAIN_FUNCTION_H
#define EXSEISDAT_UTILS_SIGNAL_PROCESSING_GAIN_FUNCTION_H

#include "exseisdat/utils/typedefs.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// @name C API
/// Interfaces visible to a C caller.
/// @{

/// @brief C API for \ref exseis::utils::signal_processing::Gain_function
/// @copydoc exseis::utils::signal_processing::Gain_function
typedef exseis_Trace_value (*exseis_Gain_function)(
  const exseis_Trace_value* signal,
  size_t window_size,
  exseis_Trace_value target_amplitude,
  size_t window_center);


/// @brief C API for
///        \ref exseis::utils::signal_processing::rectangular_rms_gain.
/// @copydoc exseis::utils::signal_processing::rectangular_rms_gain
exseis_Trace_value exseis_rectangular_rms_gain(
  const exseis_Trace_value* signal,
  size_t window_size,
  exseis_Trace_value target_amplitude,
  size_t window_center);


/// @brief C API for
///        \ref exseis::utils::signal_processing::triangular_rms_gain()
/// @copydoc exseis::utils::signal_processing::triangular_rms_gain()
exseis_Trace_value exseis_triangular_rms_gain(
  const exseis_Trace_value* signal,
  size_t window_size,
  exseis_Trace_value target_amplitude,
  size_t window_center);


/// @brief C API for \ref exseis::utils::signal_processing::mean_abs_gain()
/// @copydoc exseis::utils::signal_processing::mean_abs_gain()
exseis_Trace_value exseis_mean_abs_gain(
  const exseis_Trace_value* signal,
  size_t window_size,
  exseis_Trace_value target_amplitude,
  size_t window_center);


/// @brief C API for \ref exseis::utils::signal_processing::median_gain()
/// @copydoc exseis::utils::signal_processing::median_gain()
exseis_Trace_value exseis_median_gain(
  const exseis_Trace_value* signal,
  size_t window_size,
  exseis_Trace_value target_amplitude,
  size_t window_center);

/// @} C API


#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_SIGNAL_PROCESSING_GAIN_FUNCTION_H
