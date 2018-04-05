////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief This file contains any necessary SI units, conversion factors,
///        physical constants etc.
/// @details
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_SHARE_UNITS_HH
#define EXSEISDAT_PIOL_SHARE_UNITS_HH

#include "ExSeisDat/PIOL/typedefs.h"

/// A definition of PI.
#define PIOL_PI (3.14159265358979323846264338327950288)

namespace PIOL {

/// This namespace is being used like an enum
namespace SI {
/// Micro in SI units
constexpr unit_t Micro = 1.0e-6;
}  // namespace SI

/// Mathematical constants
namespace Math {
/// Pi
constexpr unit_t pi = PIOL_PI;

/// Pi
constexpr trace_t pi_t = PIOL_PI;
}  // namespace Math

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_SHARE_UNITS_HH
