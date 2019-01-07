////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the taper function.
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/utils/signal_processing/taper.h"
#include "exseisdat/utils/signal_processing/taper.hh"

#include <algorithm>
#include <assert.h>
#include <cmath>

namespace exseis {
namespace utils {
inline namespace signal_processing {


void taper(
  size_t signal_size,
  Trace_value* signal,
  Taper_function taper_function,
  size_t taper_size_at_begin,
  size_t taper_size_at_end)
{
    assert(
      signal_size > taper_size_at_begin && signal_size > taper_size_at_end);

    // Apply the taper at beginning of trace
    for (size_t j = 0; j < taper_size_at_begin; j++) {
        signal[j] *= taper_function(j + 1, taper_size_at_begin);
    }

    // Apply the taper at end of trace
    for (size_t j = 0; j < taper_size_at_end; j++) {
        const auto offset = signal_size - taper_size_at_end;

        signal[offset + j] *=
          taper_function(taper_size_at_end - j, taper_size_at_end);
    }
}

extern "C" void exseis_taper(
  size_t signal_size,
  exseis_Trace_value* signal,
  exseis_Taper_function taper_function,
  size_t taper_size_at_begin,
  size_t taper_size_at_end)
{
    taper(
      signal_size, signal, taper_function, taper_size_at_begin,
      taper_size_at_end);
}


}  // namespace signal_processing
}  // namespace utils
}  // namespace exseis
