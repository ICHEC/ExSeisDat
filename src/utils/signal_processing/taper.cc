////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the taper function.
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/utils/signal_processing/taper.h"

#include <algorithm>
#include <assert.h>

namespace exseis {
namespace utils {
inline namespace signal_processing {


void taper(
  size_t signal_size,
  Trace_value* signal,
  Taper_function taper_function,
  size_t left_taper_size,
  size_t right_taper_size)
{
    assert(signal_size > left_taper_size && signal_size > right_taper_size);

    // Drop muted trace region at the start, if it's there, and call this
    // function again for the truncated region.
    if (signal[0] == 0) {

        // Find the first non-zero entry
        const auto non_zero = [](auto v) { return v != 0; };

        auto* truncated_signal =
          std::find_if(signal, signal + signal_size, non_zero);
        assert(truncated_signal[0] != 0);


        // Find the new sizes
        const auto truncated_signal_size =
          signal_size - std::distance(signal, truncated_signal);

        const auto truncated_left =
          std::min(left_taper_size, truncated_signal_size);

        const auto truncated_right =
          std::min(right_taper_size, truncated_signal_size);


        // Call this function again with the new range
        return taper(
          truncated_signal_size, truncated_signal, taper_function,
          truncated_left, truncated_right);
    }


    // Apply the left taper
    for (size_t j = 0; j < left_taper_size; j++) {
        signal[j] *= taper_function(j + 1, left_taper_size);
    }

    // Apply the right taper
    for (size_t j = 0; j < right_taper_size; j++) {
        const auto offset = signal_size - right_taper_size;

        signal[offset + j] *=
          taper_function(right_taper_size - j, right_taper_size);
    }
}

extern "C" void exseis_taper(
  size_t signal_size,
  exseis_Trace_value* signal,
  exseis_Taper_function taper_function,
  size_t left_tail_size,
  size_t right_tail_size)
{
    taper(signal_size, signal, taper_function, left_tail_size, right_tail_size);
}


}  // namespace signal_processing
}  // namespace utils
}  // namespace exseis
