////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_AGCTYPE_H
#define EXSEISDAT_PIOL_AGCTYPE_H

#include <stddef.h>

/*! An enum class of the different types of automatic gain control functions.
 */
typedef size_t PIOL_AGCType;

#ifdef __cplusplus
namespace PIOL {
/// @copydoc PIOL_AGCType
typedef PIOL_AGCType AGCType;
}  // namespace PIOL
#endif  // __cplusplus

/// An enumeration of the different types of automatic gain control functions.
enum {
    /// AGC using RMS with rectangular window
    PIOL_AGCTYPE_RMS = 47,

    /// AGC using RMS with triangular window
    PIOL_AGCTYPE_RMSTri = 48,

    /// AGC using mean absolute value with rectangular window
    PIOL_AGCTYPE_MeanAbs = 49,

    /// AGC using the median value
    PIOL_AGCTYPE_Median = 50
};

#endif  // EXSEISDAT_PIOL_AGCTYPE_H
