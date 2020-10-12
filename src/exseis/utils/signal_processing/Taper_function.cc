////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the built in `Taper_function`s.
////////////////////////////////////////////////////////////////////////////////

#include "exseis/utils/signal_processing/Taper_function.hh"

#include <cmath>

namespace exseis {
inline namespace utils {
inline namespace signal_processing {


Trace_value linear_taper(Trace_value position, Trace_value width)
{
    return 1.0F - std::abs((position - width) / width);
}


Trace_value cosine_taper(Trace_value position, Trace_value width)
{
    constexpr auto pi = Trace_value(3.14159265358979323846264338327950288);
    return 0.5F + 0.5F * std::cos(pi * (position - width) / width);
}


Trace_value cosine_square_taper(Trace_value position, Trace_value width)
{
    return std::pow(cosine_taper(position, width), Trace_value(2));
}


}  // namespace signal_processing
}  // namespace utils
}  // namespace exseis
