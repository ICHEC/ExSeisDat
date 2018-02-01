////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief This file contains any necessary SI units, conversion factors,
///        physical constants etc.
/// @details
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLSHAREUNITS_INCLUDE_GUARD
#define PIOLSHAREUNITS_INCLUDE_GUARD

#define _USE_MATH_DEFINES

#include "global.hh"

#include <cmath>

namespace PIOL {

/// This namespace is being used like an enum
namespace SI {
/// Micro in SI units
constexpr unit_t Micro = 1e-6L;
}  // namespace SI

/// For physical constants
namespace Physics {
}

/// Mathematical constants
namespace Math {
/// Pi
constexpr unit_t pi = M_PI;
/// Pi
constexpr trace_t pi_t = M_PI;
}  // namespace Math

}  // namespace PIOL

#endif
