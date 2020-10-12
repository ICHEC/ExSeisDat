////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
////////////////////////////////////////////////////////////////////////////////

#include "exseis/piol/metadata/rules/Segy_rule_entry.hh"
#include "exseis/piol/segy/utils.hh"
#include "exseis/utils/encoding/number_encoding.hh"

#include <cmath>


namespace exseis {
inline namespace piol {
namespace segy {

Floating_point parse_scalar(int16_t segy_scalar)
{
    // If scale is zero, we assume unscaled, i.e. 1.
    if (segy_scalar == 0) {
        segy_scalar = 1;
    }

    // Positive segy_scalar represents multiplication by value
    if (segy_scalar > 0) {
        return static_cast<Floating_point>(segy_scalar);
    }

    // Negative segy_scalar represents division by value
    return 1 / static_cast<Floating_point>(-segy_scalar);
}

int16_t find_scalar(Floating_point value)
{
    static_assert(
        std::numeric_limits<int16_t>::max() > 10000,
        "int16_t isn't big enough to hold the value 10,000.");

    constexpr int16_t tenk = 10000;

    // First we need to determine what scale is required to store the
    // biggest decimal value of the int.
    Integer integer_part = Integer(value);
    int32_t int_part     = static_cast<int32_t>(integer_part);

    if (integer_part != int_part) {
        // Starting with the smallest scale factor, see what is the smallest
        // scale we can apply and still hold the integer portion.
        // We drop as much precision as it takes to store the most significant
        // digit.
        for (int16_t scal = 10; scal <= tenk; scal *= 10) {
            Integer v  = integer_part / scal;
            int32_t iv = static_cast<int32_t>(v);

            if (v == iv) {
                return scal;
            }
        }

        return 0;
    }

    // Get the first four digits
    Integer digits =
        std::llround(value * Floating_point(tenk)) - integer_part * tenk;

    // if the digits are all zero we don't need any scaling
    if (digits != 0) {
        // We try the most negative scale values we can first.
        //(scale = - 10000 / i)
        for (int16_t i = 1; i < tenk; i *= 10) {
            if ((digits % (i * 10)) != 0) {
                int16_t scale_factor = -tenk / i;
                // Now we test that we can still store the most significant
                // byte
                Floating_point scal = parse_scalar(scale_factor);

                // int32_t t = Integer(value / scal) -
                // digits;
                int32_t t = static_cast<int32_t>(std::lround(value / scal));
                t /= -scale_factor;

                if (t == integer_part) {
                    return scale_factor;
                }
            }
        }
    }

    return 1;
}

}  // namespace segy
}  // namespace piol
}  // namespace exseis
