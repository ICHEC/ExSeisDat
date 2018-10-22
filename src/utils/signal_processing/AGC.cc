////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   The Automatic Gain Control Operation
/// @details The algorithm finds some type of moving average (RMS, RMS with
///          Triangle window, Mean Abs, and Median) to signal amplitudes for
///          visualization purposes. It can be applied to signals independantly
///          or applies the same scalar to all signals at the same height.
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/utils/signal_processing/AGC.h"
#include "exseisdat/utils/signal_processing/AGC.hh"

#include <assert.h>
#include <cstddef>
#include <vector>

namespace exseis {
namespace utils {
inline namespace signal_processing {

void agc(
  size_t signal_size,
  Trace_value* signal,
  Gain_function gain_function,
  size_t window_size,
  Trace_value target_amplitude)
{
    // If window_size is divisible by 2, round up to nearset odd number.
    window_size = (window_size % 2 == 0 ? window_size + 1 : window_size);
    assert(signal_size > window_size);

    std::vector<Trace_value> gain(signal_size);

    size_t win2 = window_size / 2;

    // Since window_size is odd:
    assert(window_size == 2 * win2 + 1);


    // Find the gain normalization at the start of the signal
    for (size_t j = 0; j < win2; j++) {

        // The window_size starts before the beginning of the array, so it grows
        // in size from win2+1.
        // The center of the unchopped window starts at the beginning of the
        // chopped window so it starts from 0 and moves in.

        // The array is indexed at zero (for this iteration's offset) as
        // that's the lowest addressible index for the window.

        const size_t chopped_window_size   = win2 + j + 1;
        const size_t chopped_window_center = j;

        gain[j] = gain_function(
          &signal[0], chopped_window_size, target_amplitude,
          chopped_window_center);
    }

    // Find the gain normalization for the middle of the signal, where the
    // window remains unchopped.
    for (size_t j = win2; j < signal_size - win2; j++) {
        gain[j] =
          gain_function(&signal[j - win2], window_size, target_amplitude, win2);
    }

    // Find the gain normalization for the end of the signal.
    for (size_t j = signal_size - win2; j < signal_size; j++) {

        // The window is coming to the end of the array, so it shrinks from
        // the end as it approaches the boundary, down to a size of win2+1.
        // The center of the unchopped window will always be win2 into
        // the chopped window since we're chopping the end of the window.

        const size_t dist_from_end       = (signal_size - 1) - j;
        const size_t chopped_window_size = win2 + dist_from_end + 1;

        gain[j] = gain_function(
          &signal[j - win2], chopped_window_size, target_amplitude, win2);
    }

    // Apply the gain to the signals.
    for (size_t j = 0; j < signal_size; j++) {
        signal[j] *= gain[j];
    }
}

extern "C" void exseis_agc(
  size_t signal_size,
  exseis_Trace_value* signal,
  exseis_Gain_function gain_function,
  size_t window_size,
  exseis_Trace_value target_amplitude)
{
    agc(signal_size, signal, gain_function, window_size, target_amplitude);
}

}  // namespace signal_processing
}  // namespace utils
}  // namespace exseis
