////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The interface for signal scaling functions for gain control, along
///        with a number of default implementations.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_GAIN_CONTROL_GAIN_FUNCTION_H
#define EXSEISDAT_UTILS_GAIN_CONTROL_GAIN_FUNCTION_H

#include "ExSeisDat/utils/c_api_utils.h"
#include "ExSeisDat/utils/typedefs.h"

#ifdef __cplusplus
#include <type_traits>
#endif  // __cplusplus


/// @copydoc exseis::utils::Gain_function
typedef exseis_Trace_value (*exseis_Gain_function)(
  const exseis_Trace_value* signal,
  size_t window_size,
  exseis_Trace_value target_amplitude,
  size_t window_center);

#ifdef __cplusplus
namespace exseis {
namespace utils {

/// @brief Function type for signal scaling/normalization in the AGC function.
///
/// @param[in] signal           An array of signal amplitudes
/// @param[in] window_size      The window size of the gain function.
/// @param[in] target_amplitude Value to which signal is normalised.
/// @param[in] window_center    Index of the center of the window.
///
/// The value returned by this function should be a factor that will normalize
/// a signal at `signal[window_center]` to around the value `target_amplitude`,
/// smoothed over a window of size `window_size`.
///
/// That is, for the `j`th element of `signal`, and a `window_size` of
/// `2*half_win+1`, the scaling will be applied equivalent to:
/// @code
///     signal[j] *= Gain_function(
///         &signal[j - half_win], window_size,
///         target_amplitude, j);
/// @endcode
///
/// When the window extends outside the bounds of the `signal` array, it is
/// truncated to the size that is within the bounds of `signal`.
/// The `window_center` is an index into `signal` to the center of the
/// un-truncated window.
///
/// @return The normalization factor for the signal value.
///
using Gain_function = Trace_value (*)(
  const Trace_value* signal,
  size_t window_size,
  Trace_value target_amplitude,
  size_t window_center);

static_assert(
  std::is_same<Gain_function, exseis_Gain_function>::value,
  "exseis::utils::Gain_function and exseis_Gain_function are not the same type!");

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
/// @param[in] signal           An array of signal amplitudes
/// @param[in] window_size      The window size of the gain function.
/// @param[in] target_amplitude Value to which signal is normalised.
/// @param[in] window_center    Index of the center of the window.
///
/// @return The normalised signal value using RMS
///
#ifdef __cplusplus
Trace_value rectangular_RMS_gain(
  const Trace_value* signal,
  size_t window_size,
  Trace_value target_amplitude,
  size_t window_center);
#endif  // __cplusplus

/// @copydoc rectangular_RMS_gain()
EXSEISDAT_CXX_ONLY(extern "C")
exseis_Trace_value exseis_rectangular_RMS_gain(
  const exseis_Trace_value* signal,
  size_t window_size,
  exseis_Trace_value target_amplitude,
  size_t window_center);


/// @brief Find the normalised root mean square (RMS) of a signal in a
///        triangular window.
///
/// @param[in] signal           Signal amplitudes
/// @param[in] window_size      Window length
/// @param[in] target_amplitude Value to which traces are normalised
/// @param[in] window_center    Window center iterator
///
/// @return The normalised signal value using RMS with a triangular window
///
#ifdef __cplusplus
Trace_value triangular_RMS_gain(
  const Trace_value* signal,
  size_t window_size,
  Trace_value target_amplitude,
  size_t window_center);
#endif  // __cplusplus

/// @copydoc triangular_RMS_gain()
EXSEISDAT_CXX_ONLY(extern "C")
exseis_Trace_value exseis_triangular_RMS_gain(
  const exseis_Trace_value* signal,
  size_t window_size,
  exseis_Trace_value target_amplitude,
  size_t window_center);


/// @brief Find the normalised mean absolute value (MAV) of a signal in a
///        rectangular window.
///
/// @param[in] signal           Signal amplitudes
/// @param[in] window_size      Window length
/// @param[in] target_amplitude Value to which traces are normalised
/// @param[in] window_center    Window center iterator
///
/// @return The normalised signal value using MAV
///
#ifdef __cplusplus
Trace_value mean_abs_gain(
  const Trace_value* signal,
  size_t window_size,
  Trace_value target_amplitude,
  size_t window_center);
#endif  // __cplusplus

/// @copydoc mean_abs_gain()
EXSEISDAT_CXX_ONLY(extern "C")
exseis_Trace_value exseis_mean_abs_gain(
  const exseis_Trace_value* signal,
  size_t window_size,
  exseis_Trace_value target_amplitude,
  size_t window_center);


/// @brief Find the normalised median value inside a signal amplitude window.
///
/// @param[in] signal           Signal amplitudes
/// @param[in] window_size      Window length
/// @param[in] target_amplitude Value to which traces are normalised
/// @param[in] window_center    Window center iterator
///
/// @return The normalised median signal value.
///
#ifdef __cplusplus
Trace_value median_gain(
  const Trace_value* signal,
  size_t window_size,
  Trace_value target_amplitude,
  size_t window_center);
#endif  // __cplusplus

/// @copydoc median_gain()
EXSEISDAT_CXX_ONLY(extern "C")
exseis_Trace_value exseis_median_gain(
  const exseis_Trace_value* signal,
  size_t window_size,
  exseis_Trace_value target_amplitude,
  size_t window_center);

#ifdef __cplusplus
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_GAIN_CONTROL_GAIN_FUNCTION_H
