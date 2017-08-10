/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief This file contains any necessary SI units, conversion factors, physical constants
 *   etc
 *   \details
 *//*******************************************************************************************/
#ifndef PIOLSHAREUNITS_INCLUDE_GUARD
#define PIOLSHAREUNITS_INCLUDE_GUARD
#define _USE_MATH_DEFINES
#include <cmath>
#include "global.hh"

namespace PIOL {
namespace SI                            //!< This namespace is being used like an enum
{
    constexpr unit_t Micro = 1e-6L;     //!< Micro in SI units
}

namespace Physics                       //!< For physical constants
{

}

namespace Math                          //!< Mathematical constants
{
    constexpr unit_t pi = M_PI;         //!< Pi
    constexpr trace_t pi_t = M_PI;         //!< Pi
}

}
#endif
