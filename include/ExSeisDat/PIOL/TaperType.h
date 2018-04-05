////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_TAPERTYPE_H
#define EXSEISDAT_PIOL_TAPERTYPE_H

#include <stddef.h>

/*! An enum class of the different types of tapering windows.
 */
typedef size_t PIOL_TaperType;

#ifdef __cplusplus
namespace PIOL {
/// @copydoc PIOL_TaperType
typedef PIOL_TaperType TaperType;
}  // namespace PIOL
#endif  // __cplusplus

/// An enumeration of the different types of tapering windows.
enum {
    /// Taper using a linear ramp
    PIOL_TAPERTYPE_Linear = 44,

    /// Taper using a cos ramp
    PIOL_TAPERTYPE_Cos = 45,

    /// Taper using a cos^2 ramp
    PIOL_TAPERTYPE_CosSqr = 46
};

#endif  // EXSEISDAT_PIOL_TAPERTYPE_H
