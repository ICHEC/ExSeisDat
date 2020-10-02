////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the built in `Taper_function`s.
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/utils/signal_processing/Taper_function.hh"

#include <cmath>

namespace exseis {
namespace utils {
inline namespace signal_processing {


Trace_value linear_taper(Trace_value weight, Trace_value ramp)
{
    return 1.0f - std::abs((weight - ramp) / ramp);
}


Trace_value cosine_taper(Trace_value weight, Trace_value ramp)
{
    constexpr auto pi = Trace_value(3.14159265358979323846264338327950288);
    return 0.5f + 0.5f * std::cos(pi * (weight - ramp) / ramp);
}


Trace_value cosine_square_taper(Trace_value weight, Trace_value ramp)
{
    return std::pow(cosine_taper(weight, ramp), Trace_value(2));
}


}  // namespace signal_processing
}  // namespace utils
}  // namespace exseis
