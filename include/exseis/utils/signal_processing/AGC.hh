////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The Automatic Gain Control Operation
/// @details This function allows for automatic gain control to be applied to
///          traces in order to equalize signal amplitude for display purposes.
///          It uses a user defined window to adjust the gain using a user
///          defined normalization value.
///
///          Gain is normalised against one of:
///             a) RMS amplitude
///             b) RMS amplitude with triangle window
///             c) Mean Absolute Value amplitude
///             d) Median amplitude.
///
///          agc is intended to be applied to trace data.
///          agc can be performed on individual traces or entire gathers, where
///          the same gain is applied to the jth sample in every trace. The
///          window has a spatial componet.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEIS_UTILS_SIGNAL_PROCESSING_AGC_HH
#define EXSEIS_UTILS_SIGNAL_PROCESSING_AGC_HH

#include "exseis/utils/signal_processing/Gain_function.hh"
#include "exseis/utils/types/typedefs.hh"


namespace exseis {
inline namespace utils {
inline namespace signal_processing {

/// @brief Apply automatic gain control to a set of tapers --> used for actual
///        operation during output
///
/// @param[in]    signal_size      The number of samples in a signal
/// @param[in,out] signal           Array of the signal data.
/// @param[in]    gain_function    Statistical function which returns a scaled
///                                value for a signal.
/// @param[in]    window_size      Length of the agc window
/// @param[in]    target_amplitude Value to which signal are normalised
///
void agc(
    size_t signal_size,
    Trace_value* signal,
    Gain_function gain_function,
    size_t window_size,
    Trace_value target_amplitude);

}  // namespace signal_processing
}  // namespace utils
}  // namespace exseis

#endif  // EXSEIS_UTILS_SIGNAL_PROCESSING_AGC_HH
