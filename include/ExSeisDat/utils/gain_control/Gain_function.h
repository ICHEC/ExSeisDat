////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The interface for signal scaling functions for gain control, along
///        with a number of default implementations.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_GAIN_CONTROL_GAIN_FUNCTION_H
#define EXSEISDAT_UTILS_GAIN_CONTROL_GAIN_FUNCTION_H

#include "ExSeisDat/PIOL/c_api_utils.h"
#include "ExSeisDat/utils/typedefs.h"


/// Function type for signal scaling in the AGC function.
typedef PIOL_trace_t (*PIOL_Gain_function)(
  const PIOL_trace_t* signal,
  size_t window,
  PIOL_trace_t target_amplitude,
  size_t window_center);

#ifdef __cplusplus
namespace PIOL {
/// @copydoc PIOL_Gain_function
using Gain_function = PIOL_Gain_function;
}  // namespace PIOL
#endif  // __cplusplus


#ifdef __cplusplus
namespace PIOL {
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
trace_t rectangular_RMS_gain(
  const trace_t* signal,
  size_t window,
  trace_t target_amplitude,
  size_t window_center);
#endif  // __cplusplus

/// @copydoc rectangular_RMS_gain()
EXSEISDAT_CXX_ONLY(extern "C")
PIOL_trace_t PIOL_rectangular_RMS_gain(
  const PIOL_trace_t* signal,
  size_t window,
  PIOL_trace_t target_amplitude,
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
trace_t triangular_RMS_gain(
  const trace_t* signal,
  size_t window,
  trace_t target_amplitude,
  size_t window_center);
#endif  // __cplusplus

/// @copydoc triangular_RMS_gain()
EXSEISDAT_CXX_ONLY(extern "C")
PIOL_trace_t PIOL_triangular_RMS_gain(
  const PIOL_trace_t* signal,
  size_t window,
  PIOL_trace_t target_amplitude,
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
trace_t mean_abs_gain(
  const trace_t* signal,
  size_t window,
  trace_t target_amplitude,
  size_t window_center);
#endif  // __cplusplus

/// @copydoc mean_abs_gain()
EXSEISDAT_CXX_ONLY(extern "C")
PIOL_trace_t PIOL_mean_abs_gain(
  const PIOL_trace_t* signal,
  size_t window,
  PIOL_trace_t target_amplitude,
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
trace_t median_gain(
  const trace_t* signal,
  size_t window,
  trace_t target_amplitude,
  size_t window_center);
#endif  // __cplusplus

/// @copydoc median_gain()
EXSEISDAT_CXX_ONLY(extern "C")
PIOL_trace_t PIOL_median_gain(
  const PIOL_trace_t* signal,
  size_t window,
  PIOL_trace_t target_amplitude,
  size_t window_center);

#ifdef __cplusplus
}  // namespace PIOL
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_GAIN_CONTROL_GAIN_FUNCTION_H
