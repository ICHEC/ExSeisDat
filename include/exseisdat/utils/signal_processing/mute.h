////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Application of mutes to either end of the singal, then applies a
/// @      taper function to a signal.
////////////////////////////////////////////////////////////////////////////////

#ifndef EXSEISDAT_UTILS_SIGNAL_PROCESSING_MUTE_H
#define EXSEISDAT_UTILS_SIGNAL_PROCESSING_MUTE_H

#include "exseisdat/utils/signal_processing/Taper_function.h"
#include "exseisdat/utils/types/typedefs.h"

#include <stddef.h>

#ifdef __cplusplus

namespace exseis {
namespace utils {
inline namespace signal_processing {

extern "C" {
#endif  // __cplusplus

/// @name C API
/// @{

/// @brief C API for exseis::utils::signal_processing::mute
/// @copydoc exseis::utils::signal_processing::mute
void exseis_mute(
    size_t signal_size,
    exseis_Trace_value* signal,
    exseis_Taper_function taper_function,
    size_t mute_size_at_begin,
    size_t taper_size_at_begin,
    size_t taper_size_at_end,
    size_t mute_size_at_end);

/// @} C API

#ifdef __cplusplus
}  // extern "C"

}  // namespace signal_processing
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_SIGNAL_PROCESSING_MUTE_H
