////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C API for \ref exseisdat/utils/signal_processing/taper.hh
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_H
#define EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_H

#include "exseisdat/utils/signal_processing/Taper_function.h"
#include "exseisdat/utils/typedefs.h"

#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// @name C API
/// @{

/// @brief C API for exseis::utils::signal_processing::taper
/// @copydoc exseis::utils::signal_processing::taper
void exseis_taper(
    size_t signal_size,
    exseis_Trace_value* signal,
    exseis_Taper_function taper_function,
    size_t taper_size_at_begin,
    size_t taper_size_at_end);

/// @} C API


#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_H
