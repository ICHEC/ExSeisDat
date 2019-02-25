////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C API for \ref exseisdat/utils/signal_processing/AGC.hh
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_SIGNAL_PROCESSING_AGC_H
#define EXSEISDAT_UTILS_SIGNAL_PROCESSING_AGC_H

#include "exseisdat/utils/signal_processing/Gain_function.h"
#include "exseisdat/utils/typedefs.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// @name C API
/// @{

/// @brief C API interface for \ref exseis::utils::signal_processing::agc
/// @copydoc exseis::utils::signal_processing::agc
void exseis_agc(
    size_t signal_size,
    exseis_Trace_value* signal,
    exseis_Gain_function gain_function,
    size_t window_size,
    exseis_Trace_value target_amplitude);

/// @} C API

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_SIGNAL_PROCESSING_AGC_H
