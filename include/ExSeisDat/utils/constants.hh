////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief This file contains any necessary SI units, conversion factors,
///        physical constants etc. used in the project.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_CONSTANTS_HH
#define EXSEISDAT_UTILS_CONSTANTS_HH

#include "ExSeisDat/utils/typedefs.h"

/// A definition of PI.
#define EXSEISDAT_PI (3.14159265358979323846264338327950288)

namespace exseis {
namespace utils {
inline namespace constants {

/// The micro- prefix in SI units
constexpr unit_t micro = 1.0e-6;

/// Pi
constexpr unit_t pi = EXSEISDAT_PI;

/// Pi
constexpr trace_t pi_t = EXSEISDAT_PI;

}  // namespace constants
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_CONSTANTS_HH
