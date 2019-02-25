////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Application of mutes to either end of the singal, then applies a
///        taper function to a signal.
////////////////////////////////////////////////////////////////////////////////

#ifndef EXSEISDAT_UTILS_SIGNAL_PROCESSING_MUTE_HH
#define EXSEISDAT_UTILS_SIGNAL_PROCESSING_MUTE_HH

#include "exseisdat/utils/signal_processing/Taper_function.hh"
#include "exseisdat/utils/typedefs.hh"

#include <cstddef>

namespace exseis {
namespace utils {
inline namespace signal_processing {

/// @brief Apply mutes and taper to a signal.
///
/// This mute function sets regions at the start and end of a signal to zero
/// as specified by the user.
///
/// Following the implentation of these mute regions, a taper_function will
/// apply a taper beginning from the first non-zero entry.
///
/// @param[in] signal_size          The number of samples in the signal
/// @param[in] signal               An array of amplitudes
/// @param[in] taper_function       Weight function for the taper ramp
/// @param[in] mute_size_at_begin   Length of the begin mute
/// @param[in] taper_size_at_begin  Length of the begin tail of the taper
/// @param[in] mute_size_at_end     Length of the end mute
/// @param[in] taper_size_at_end    Length of the end tail of the taper
///
/// @note C API: exseis_mute
///

void mute(
    size_t signal_size,
    Trace_value* signal,
    Taper_function taper_function,
    size_t mute_size_at_begin,
    size_t taper_size_at_begin,
    size_t mute_size_at_end,
    size_t taper_size_at_end);

}  // namespace signal_processing
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_SIGNAL_PROCESSING_MUTE_H
