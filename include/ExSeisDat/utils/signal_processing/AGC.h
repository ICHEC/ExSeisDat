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
///          AGC is intended to be applied to trace data.
///          AGC can be performed on individual traces or entire gathers, where
///          the same gain is applied to the jth sample in every trace. The
///          window has a spatial componet.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_SIGNAL_PROCESSING_AGC_H
#define EXSEISDAT_UTILS_SIGNAL_PROCESSING_AGC_H

#include "ExSeisDat/utils/c_api_utils.h"
#include "ExSeisDat/utils/signal_processing/Gain_function.h"
#include "ExSeisDat/utils/typedefs.h"


///
/// @namespace exseis::utils::signal_processing
///
/// @brief Functions for processing seismic signals.
///


#ifdef __cplusplus
namespace exseis {
namespace utils {
inline namespace signal_processing {
#endif  // __cplusplus


/// @brief Apply automatic gain control to a set of tapers --> used for actual
///        operation during output
///
/// @param[in]    signal_size      The number of samples in a signal
/// @param[inout] signal           Array of the signal data.
/// @param[in]    gain_function    Statistical function which returns a scaled
///                                value for a signal.
/// @param[in]    window_size      Length of the agc window
/// @param[in]    target_amplitude Value to which signal are normalised
///
/// @remark C API: \ref exseis_AGC
///
#ifdef __cplusplus
void AGC(
  size_t signal_size,
  Trace_value* signal,
  Gain_function gain_function,
  size_t window_size,
  Trace_value target_amplitude);
#endif  // __cplusplus

/// @name C API
/// @{

/// @brief C API interface for \ref AGC
/// @copydoc AGC
EXSEISDAT_CXX_ONLY(extern "C")
void exseis_AGC(
  size_t signal_size,
  exseis_Trace_value* signal,
  exseis_Gain_function gain_function,
  size_t window_size,
  exseis_Trace_value target_amplitude);

/// @} C API

#ifdef __cplusplus
}  // inline namespace signal_processing
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_SIGNAL_PROCESSING_AGC_H
