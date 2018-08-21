////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The `taper` function, applies a taper to a signal.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_H
#define EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_H

#include "ExSeisDat/utils/c_api_utils.h"
#include "ExSeisDat/utils/signal_processing/Taper_function.h"
#include "ExSeisDat/utils/typedefs.h"

#ifdef __cplusplus
namespace exseis {
namespace utils {
inline namespace signal_processing {
#endif  // __cplusplus


/// @brief Apply a taper to a signal.
///
/// This taper_function will drop any initial zeros in the signal and apply the
/// taper beginning from the first non-zero entry.
///
/// @param[in] signal_size     The number of samples in the signal
/// @param[in] signal          An array of amplitudes
/// @param[in] taper_function  Weight function for the taper ramp
/// @param[in] left_tail_size  Length of the left tail of the taper
/// @param[in] right_tail_size Length of the right tail of the taper
///
/// @note C API: exseis_taper
///
#ifdef __cplusplus
void taper(
  size_t signal_size,
  Trace_value* signal,
  Taper_function taper_function,
  size_t left_tail_size,
  size_t right_tail_size);
#endif  // __cplusplus

/// @name C API
/// @{

/// @copydoc exseis::utils::signal_processing::taper
EXSEISDAT_CXX_ONLY(extern "C")
void exseis_taper(
  size_t signal_size,
  exseis_Trace_value* signal,
  exseis_Taper_function taper_function,
  size_t left_tail_size,
  size_t right_tail_size);

/// @} C API


#ifdef __cplusplus
}  // inline namespace signal_processing
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_SIGNAL_PROCESSING_TAPER_H
