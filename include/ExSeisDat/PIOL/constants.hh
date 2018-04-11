////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief This file contains any necessary SI units, conversion factors,
///        physical constants etc. used in the project.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_CONSTANTS_HH
#define EXSEISDAT_PIOL_CONSTANTS_HH

#include "ExSeisDat/utils/typedefs.h"

/// A definition of PI.
#define PIOL_PI (3.14159265358979323846264338327950288)

namespace PIOL {

/// The micro- prefix in SI units
constexpr unit_t micro = 1.0e-6;

/// Pi
constexpr unit_t pi = PIOL_PI;

/// Pi
constexpr trace_t pi_t = PIOL_PI;

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_CONSTANTS_HH
