////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The interface for signal scaling functions for gain control, along
///        with a number of default implementations.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_SIGNAL_PROCESSING_GAIN_FUNCTION_HH
#define EXSEISDAT_UTILS_SIGNAL_PROCESSING_GAIN_FUNCTION_HH

#include "exseisdat/utils/types/typedefs.hh"

#include <cstddef>

namespace exseis {
namespace utils {
inline namespace signal_processing {

/// @brief Function type for signal scaling/normalization in the agc function.
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


/// @name Gain Functions
/// @{

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
Trace_value rectangular_rms_gain(
    const Trace_value* signal,
    size_t window_size,
    Trace_value target_amplitude,
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
Trace_value triangular_rms_gain(
    const Trace_value* signal,
    size_t window_size,
    Trace_value target_amplitude,
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
Trace_value mean_abs_gain(
    const Trace_value* signal,
    size_t window_size,
    Trace_value target_amplitude,
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
Trace_value median_gain(
    const Trace_value* signal,
    size_t window_size,
    Trace_value target_amplitude,
    size_t window_center);

/// @} Gain Functions


}  // namespace signal_processing
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_SIGNAL_PROCESSING_GAIN_FUNCTION_HH
