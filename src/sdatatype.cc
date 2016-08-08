#include <stdint.h>
#include "global.hh"
#include "share/datatype.hh"
namespace PIOL {
float tofloat(const uint32_t i)
{
    union {
        float f;
        uint32_t i;
    } n;
    n.i = i;
    return n.f;
}

uint32_t toint(const float f)
{
    union {
        float f;
        uint32_t i;
    } n;
    n.f = f;
    return n.i;
}

//TODO: Haven't done anything for underflow, overflow or nans
//TODO: Not extensively tested yet
float convertIBMtoIEEE(const uint32_t i)
{
    // The first 24 bits make up the fraction. If it's zero we can stop
    uint32_t frac = i & 0xFFFFFF;
    if (frac == 0)
        return float(0);

    //Exact the sign of the fraction (last bit)
    uint32_t sign = (i >> 31) & 0x01;

    //The exponent is 7 bits. A bias of 64 is removed.
    //The exponent is in base 16, shift by 3 puts it into base 2.
    uint32_t exp = ((((i >> 24) & 0x7F) - 64) << 3);

    //Now we can Convert to IEEE

    //3 leading bits can be zero as a result of 16 base exponent. We are converting to 2 base.
    //We compensate here by using a 3 bit lookup table to count the leading zeroes.
    static const uint32_t clz[] = {3, 2, 1, 1, 0, 0, 0, 0};
    uint32_t shift = clz[frac >> 21];
    //IEEE Has an implicit first bit since it must be positive.
    frac = (frac << shift) ^ (1 << 23);
    exp -= shift;

    //Shift components to the positions of IEEE float. 127 is the IEEE bias
    return tofloat(sign << 31 | (exp+127) << 23 | frac);
}
}
