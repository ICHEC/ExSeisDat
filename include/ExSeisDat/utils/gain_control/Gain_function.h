////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The interface for signal scaling functions for gain control, along
///        with a number of default implementations.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_GAIN_CONTROL_GAIN_FUNCTION_H
#define EXSEISDAT_UTILS_GAIN_CONTROL_GAIN_FUNCTION_H

#include "ExSeisDat/utils/c_api_utils.h"
#include "ExSeisDat/utils/typedefs.h"


/// Function type for signal scaling in the AGC function.
typedef exseis_Trace_value (*exseis_Gain_function)(
  const exseis_Trace_value* signal,
  size_t window,
  exseis_Trace_value target_amplitude,
  size_t window_center);

#ifdef __cplusplus
namespace exseis {
namespace utils {
/// @copydoc exseis_Gain_function
using Gain_function = exseis_Gain_function;
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus


#ifdef __cplusplus
namespace exseis {
namespace utils {
#endif  // __cplusplus


/// @brief Find the normalised root mean square (RMS) of a signal in a
///        rectangular window.
///
/// @param[in] signal           Signal amplitudes
/// @param[in] window           Window length
/// @param[in] target_amplitude Value to which signal is normalised
/// @param[in] window_center    Window center iterator
///
/// @return The normalised signal value using RMS
///
#ifdef __cplusplus
exseis::utils::Trace_value rectangular_RMS_gain(
  const exseis::utils::Trace_value* signal,
  size_t window,
  exseis::utils::Trace_value target_amplitude,
  size_t window_center);
#endif  // __cplusplus

/// @copydoc rectangular_RMS_gain()
EXSEISDAT_CXX_ONLY(extern "C")
exseis_Trace_value exseis_rectangular_RMS_gain(
  const exseis_Trace_value* signal,
  size_t window,
  exseis_Trace_value target_amplitude,
  size_t window_center);


/// @brief Find the normalised root mean square (RMS) of a signal in a
///        triangular window.
///
/// @param[in] signal           Signal amplitudes
/// @param[in] window           Window length
/// @param[in] target_amplitude Value to which traces are normalised
/// @param[in] window_center    Window center iterator
///
/// @return The normalised signal value using RMS with a triangular window
///
#ifdef __cplusplus
exseis::utils::Trace_value triangular_RMS_gain(
  const exseis::utils::Trace_value* signal,
  size_t window,
  exseis::utils::Trace_value target_amplitude,
  size_t window_center);
#endif  // __cplusplus

/// @copydoc triangular_RMS_gain()
EXSEISDAT_CXX_ONLY(extern "C")
exseis_Trace_value exseis_triangular_RMS_gain(
  const exseis_Trace_value* signal,
  size_t window,
  exseis_Trace_value target_amplitude,
  size_t window_center);


/// @brief Find the normalised mean absolute value (MAV) of a signal in a
///        rectangular window.
///
/// @param[in] signal           Signal amplitudes
/// @param[in] window           Window length
/// @param[in] target_amplitude Value to which traces are normalised
/// @param[in] window_center    Window center iterator
///
/// @return The normalised signal value using MAV
///
#ifdef __cplusplus
exseis::utils::Trace_value mean_abs_gain(
  const exseis::utils::Trace_value* signal,
  size_t window,
  exseis::utils::Trace_value target_amplitude,
  size_t window_center);
#endif  // __cplusplus

/// @copydoc mean_abs_gain()
EXSEISDAT_CXX_ONLY(extern "C")
exseis_Trace_value exseis_mean_abs_gain(
  const exseis_Trace_value* signal,
  size_t window,
  exseis_Trace_value target_amplitude,
  size_t window_center);


/// @brief Find the normalised median value inside a signal amplitude window.
///
/// @param[in] signal           Signal amplitudes
/// @param[in] window           Window length
/// @param[in] target_amplitude Value to which traces are normalised
/// @param[in] window_center    Window center iterator
///
/// @return The normalised median signal value.
///
#ifdef __cplusplus
exseis::utils::Trace_value median_gain(
  const exseis::utils::Trace_value* signal,
  size_t window,
  exseis::utils::Trace_value target_amplitude,
  size_t window_center);
#endif  // __cplusplus

/// @copydoc median_gain()
EXSEISDAT_CXX_ONLY(extern "C")
exseis_Trace_value exseis_median_gain(
  const exseis_Trace_value* signal,
  size_t window,
  exseis_Trace_value target_amplitude,
  size_t window_center);

#ifdef __cplusplus
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_GAIN_CONTROL_GAIN_FUNCTION_H
