////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date October 2016
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "file/segymd.hh"
#include "global.hh"

#include <cmath>

namespace PIOL {
namespace File {

geom_t scaleConv(int16_t scale)
{
    scale = (!scale ? 1 : scale);
    return (scale > 0 ? geom_t(scale) : geom_t(1) / geom_t(-scale));
}

int16_t deScale(const geom_t val)
{
    constexpr llint tenk = 10000LL;
    // First we need to determine what scale is required to store the
    // biggest decimal value of the int.
    llint llintpart = llint(val);
    int32_t intpart = llintpart;
    if (llintpart != intpart) {
        // Starting with the smallest scale factor, see what is the smallest
        // scale we can apply and still hold the integer portion.
        // We drop as much precision as it takes to store the most significant
        // digit.
        for (int32_t scal = 10; scal <= tenk; scal *= 10) {
            llint v    = llintpart / scal;
            int32_t iv = v;
            if (v == iv) return scal;
        }
        return 0;
    }
    else {
        // Get the first four digits
        llint digits = std::llround(val * geom_t(tenk)) - llintpart * tenk;
        // if the digits are all zero we don't need any scaling
        if (digits != 0) {
            // We try the most negative scale values we can first.
            //(scale = - 10000 / i)
            for (int32_t i = 1; i < tenk; i *= 10) {
                if (digits % (i * 10)) {
                    int16_t scaleFactor = -tenk / i;
                    // Now we test that we can still store the most significant
                    // byte
                    geom_t scal = scaleConv(scaleFactor);

                    // int32_t t = llint(val / scal) - digits;
                    int32_t t = std::lround(val / scal);
                    t /= -scaleFactor;

                    if (t == llintpart) return scaleFactor;
                }
            }
        }
        return 1;
    }
}

}  // namespace File
}  // namespace PIOL
