#include <stdint.h>
#include <arpa/inet.h>

#warning remove
#include <iostream>
#include <assert.h>
#include "global.hh"
#include "share/datatype.hh"
namespace PIOL {
void reverse4Bytes(uchar * src)
{
    std::swap(src[0], src[3]);
    std::swap(src[1], src[2]);
}

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
float convertIBMtoIEEE(const float f, bool bigEndian)
{
    uint32_t i = toint(f);
    if (bigEndian)
        i = ntohl(i);

    //The first 24 bits make up the fraction. If it's zero we can stop
    //Converting from base 16 to base 2.
    uint32_t frac = (i & 0x00FFFFFF);
    //uint32_t frac = (i & 0x00FFFFFF) << 2;
    if (frac == 0)
        return float(0);

    //Extract the sign of the fraction (last bit)
    uint32_t sign = (i >> 31) & 0x01;

    //The exponent is 7 bits. A bias of 64 is removed.
    //The exponent is in base 16, shift by 3 puts it into base 2.
    //int32_t exp = (int32_t((i >> 24) & 0x7F) - 64) << 2;
    int32_t exp = ((int32_t((i >> 24) & 0x7F) - 64) << 2) - 1;

    //int32_t exp = (int32_t((i >> 24) & 0x7F) - 64) << 2;
   // uint32_t exp = (((i >> 24) & 0x7F)-64) << 2;

    //Now we can Convert to IEEE
    static const uint32_t nlz[] = {0, 1, 2, 2, 3, 3, 3, 3};
    int32_t shift = nlz[frac >> 24];
    frac >>= shift;
    exp += shift;

    //3 leading bits can be zero as a result of 16 base exponent. We are converting to 2 base.
    //We compensate here by using a 3 bit lookup table to count the leading zeroes.
    static const uint32_t clz[] = {3, 2, 1, 1, 0, 0, 0, 0};
    shift = clz[frac >> 21];
    frac <<= shift;
    exp -= shift;

//    std::cout << shift << " " << exp << " " << (frac >> 21) << std::endl;

    frac = frac & 0x7FFFFF;

    //Shift components to the positions of IEEE float. 127 is the IEEE bias
    return tofloat(sign << 31 | ((exp + 127) & 0x000000FF) << 23 | frac);
}
}
