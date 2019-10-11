////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The `taper` function, applies a taper to a signal.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_HH
#define EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_HH

#include "exseisdat/utils/signal_processing/Taper_function.hh"
#include "exseisdat/utils/types/typedefs.hh"

#include <cstddef>

namespace exseis {
namespace utils {
inline namespace signal_processing {


/// @brief Apply a taper to a signal.
///
/// This taper_function will drop any initial zeros in the signal and apply the
/// taper beginning from the first non-zero entry.
///
/// @param[in] signal_size          The number of samples in the signal
/// @param[in] signal               An array of amplitudes
/// @param[in] taper_function       Weight function for the taper ramp
/// @param[in] taper_size_at_begin  Length of the begin tail of the taper
/// @param[in] taper_size_at_end    Length of the end tail of the taper
///
/// @remark C API: exseis_taper
///
void taper(
    size_t signal_size,
    Trace_value* signal,
    Taper_function taper_function,
    size_t taper_size_at_begin,
    size_t taper_size_at_end);


}  // namespace signal_processing
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_HH
