////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the mute function.
////////////////////////////////////////////////////////////////////////////////

#include "exseis/utils/signal_processing/mute.hh"

#include "exseis/utils/signal_processing/Taper_function.hh"
#include "exseis/utils/signal_processing/taper.hh"

#include <algorithm>
#include <cassert>

namespace exseis {
inline namespace utils {
inline namespace signal_processing {

void mute(
    size_t signal_size,
    Trace_value* signal,
    Taper_function taper_function,
    size_t mute_size_at_begin,
    size_t taper_size_at_begin,
    size_t mute_size_at_end,
    size_t taper_size_at_end)

{
    assert(signal_size > mute_size_at_begin + taper_size_at_begin);
    assert(signal_size > taper_size_at_end + mute_size_at_end);


    // Apply the mute at the beginning of the trace
    for (size_t j = 0; j < mute_size_at_begin; j++) {
        signal[j] = 0;
    }

    // Apply the taper at the beginning of the trace, keeping the muted
    // section at the beginning of the trace separate

    for (size_t j = mute_size_at_begin;
         j < taper_size_at_begin + mute_size_at_begin; j++) {
        signal[j] *=
            taper_function(j - mute_size_at_begin, taper_size_at_begin);
    }

    // Apply the taper at the end of the trace, keeping the muted section
    // at the end of the trace separate

    for (size_t j = signal_size - taper_size_at_end - mute_size_at_end;
         j < signal_size - mute_size_at_end; j++) {
        signal[j] *= taper_function(
            signal_size - (j + 1) - mute_size_at_end, taper_size_at_end);
    }

    // Apply the mute at the end of the trace
    for (size_t j = signal_size - mute_size_at_end; j < signal_size; j++) {
        signal[j] = 0;
    }
}

}  // namespace signal_processing
}  // namespace utils
}  // namespace exseis
