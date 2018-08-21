///
/// @file
/// @brief Implementation for the build in `Gain_function`s.
///

#include "ExSeisDat/utils/signal_processing/Gain_function.h"
#include "ExSeisDat/utils/typedefs.h"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <vector>

namespace exseis {
namespace utils {
inline namespace signal_processing {

Trace_value rectangular_RMS_gain(
  const Trace_value* signal,
  size_t window_size,
  Trace_value target_amplitude,
  size_t)
{
    Trace_value amp = 0;
    for (size_t j = 0; j < window_size; j++) {
        amp += signal[j] * signal[j];
    }
    assert(amp != 0);

    const auto non_zero = [](Trace_value i) { return i != 0; };
    const size_t num    = std::count_if(signal, &signal[window_size], non_zero);

    return target_amplitude / std::sqrt(amp / std::max<size_t>(1, num));
}

extern "C" Trace_value exseis_rectangular_RMS_gain(
  const exseis_Trace_value* signal,
  size_t window_size,
  exseis_Trace_value target_amplitude,
  size_t)
{
    return rectangular_RMS_gain(signal, window_size, target_amplitude, 0);
}


Trace_value triangular_RMS_gain(
  const Trace_value* signal,
  size_t window_size,
  Trace_value target_amplitude,
  size_t window_center)
{
    assert(window_size > 0);
    assert(window_size > window_center);

    // When the window center is near the start or the end of the range of
    // signals, the window can go outside this range.
    //
    // Say we have the following setup:
    //
    // signals:          [****************]
    // window:        [-------]
    // window center:     *
    // scaling:           ^
    //                   / \
    //                  /   \
    //                 /     \
    //
    // We treat the array of signals as though it's padded with zeros:
    //
    // signals:        [00****************]
    // window:        [-------]
    // window center:     X
    // scaling:           ^
    //                   / \
    //                  /   \
    //                 /     \
    //
    // But the view we pass into this function is:
    //
    // signals:          [****************]
    // window:          [-----]
    // window center:     ^
    // scaling:          / \
    //                      \
    //                       \
    //
    // That is, the signals are passed in from as low a value as possible,
    // the window size is set to the number of addressable elements in the
    // signal, and the window center is set to some non-central position.
    //
    // So, we can't rely on the window center to actually be at the center,
    // but we know the center to *one* of the ends will be half the width,
    // and in particular, whichever is bigger will be the half-width.
    //

    // 0 to window_center
    const size_t left_width = window_center;

    // window_center to window_size - 1.
    const size_t right_width = (window_size - 1) - window_center;

    const Trace_value half_width = std::max(left_width, right_width);

    Trace_value amp = 0;
    for (size_t j = 0; j < window_size; j++) {
        // A signed-sensitive std::abs(j - window_center)
        const auto distance_from_center =
          (j > window_center) ? j - window_center : window_center - j;

        // Linear interpolation:
        //      0 at distance_from_center == half_width
        //      1 at distance_from_center == 0
        const auto scaling = 1 - distance_from_center / half_width;

        const auto scaled_signal = signal[j] * scaling;

        amp += scaled_signal * scaled_signal;
    }
    assert(amp != 0);

    const auto non_zero = [](Trace_value i) { return i != 0; };
    const size_t num    = std::count_if(signal, &signal[window_size], non_zero);

    return target_amplitude / std::sqrt(amp / std::max<size_t>(1, num));
}

extern "C" Trace_value exseis_triangular_RMS_gain(
  const exseis_Trace_value* signal,
  size_t window_size,
  exseis_Trace_value target_amplitude,
  size_t window_center)
{
    return triangular_RMS_gain(
      signal, window_size, target_amplitude, window_center);
}


Trace_value mean_abs_gain(
  const Trace_value* signal,
  size_t window_size,
  Trace_value target_amplitude,
  size_t)
{
    assert(window_size > 0);

    const Trace_value amp =
      std::accumulate(signal, signal + window_size, Trace_value(0));
    assert(amp != 0);

    const auto non_zero = [](Trace_value i) { return i != Trace_value(0); };
    const size_t num    = std::count_if(signal, &signal[window_size], non_zero);

    return target_amplitude / (std::abs(amp) / std::max<size_t>(1, num));
}

extern "C" Trace_value exseis_mean_abs_gain(
  const exseis_Trace_value* signal,
  size_t window_size,
  exseis_Trace_value target_amplitude,
  size_t)
{
    return mean_abs_gain(signal, window_size, target_amplitude, 0);
}


Trace_value median_gain(
  const Trace_value* signal,
  size_t window_size,
  Trace_value target_amplitude,
  size_t)
{
    // This could be optimised with std::nth_element if required.
    std::vector<Trace_value> signalTmp(signal, &signal[window_size]);
    std::sort(signalTmp.begin(), signalTmp.end());

    if (window_size % 2 == 0) {
        const auto amp =
          ((signalTmp[window_size / 2] + signalTmp[window_size / 2 + 1]) / 2);
        assert(amp != 0);

        return target_amplitude / amp;
    }
    else {
        const auto amp = signalTmp[window_size / 2];
        assert(amp != 0);

        return target_amplitude / amp;
    }
}

extern "C" Trace_value exseis_median_gain(
  const exseis_Trace_value* signal,
  size_t window_size,
  exseis_Trace_value target_amplitude,
  size_t)
{
    return median_gain(signal, window_size, target_amplitude, 0);
}

}  // namespace signal_processing
}  // namespace utils
}  // namespace exseis
