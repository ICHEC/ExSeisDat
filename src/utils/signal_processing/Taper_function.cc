////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the built in `Taper_function`s.
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/utils/signal_processing/Taper_function.h"
#include "exseisdat/utils/signal_processing/Taper_function.hh"

#include <cmath>

namespace exseis {
namespace utils {
inline namespace signal_processing {


Trace_value linear_taper(Trace_value weight, Trace_value ramp)
{
    return 1.0f - std::abs((weight - ramp) / ramp);
}

extern "C" exseis_Trace_value exseis_linear_taper(
    exseis_Trace_value weight, exseis_Trace_value ramp)
{
    return linear_taper(weight, ramp);
}


Trace_value cosine_taper(Trace_value weight, Trace_value ramp)
{
    constexpr auto pi = Trace_value(3.14159265358979323846264338327950288);
    return 0.5f + 0.5f * std::cos(pi * (weight - ramp) / ramp);
}

extern "C" exseis_Trace_value exseis_cosine_taper(
    exseis_Trace_value weight, exseis_Trace_value ramp)
{
    return cosine_taper(weight, ramp);
}


Trace_value cosine_square_taper(Trace_value weight, Trace_value ramp)
{
    return std::pow(cosine_taper(weight, ramp), Trace_value(2));
}

extern "C" exseis_Trace_value exseis_cosine_square_taper(
    exseis_Trace_value weight, exseis_Trace_value ramp)
{
    return cosine_square_taper(weight, ramp);
}


}  // namespace signal_processing
}  // namespace utils
}  // namespace exseis
