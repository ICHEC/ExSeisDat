////////////////////////////////////////////////////////////////////////////////
///  \file
///  \author Cathal O Broin - cathal@ichec.ie - first commit
///  \copyright TBD. Do not distribute
///  \date August 2016
///  \brief This file performs a variety of bit operations and conversions on
///  datatypes \details
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/global.hh"
#include "ExSeisDat/PIOL/share/datatype.hh"

#include <cstring>

namespace PIOL {

void reverse4Bytes(uchar* src)
{
    std::swap(src[0], src[3]);
    std::swap(src[1], src[2]);
}

float tofloat(uint32_t i)
{
    static_assert(sizeof(uint32_t) == sizeof(float), "float isn't 32 bits!");

    float f;
    std::memcpy(&f, &i, sizeof(float));

    return f;
}

uint32_t toint(float f)
{
    static_assert(sizeof(float) == sizeof(uint32_t), "float isn't 32 bits!");

    uint32_t i;
    std::memcpy(&i, &f, sizeof(uint32_t));

    return i;
}


FloatComponents from_IBM(uint32_t ibm_float, bool big_endian)
{
    // Get the IBM float in the native endian order
    const uint32_t ibm_bits = [=]() {
        const uint8_t* bits = reinterpret_cast<const uint8_t*>(&ibm_float);

        if (big_endian == true) {
            return (bits[0] << 24) | (bits[1] << 16) | (bits[2] << 8)
                   | (bits[3] << 0);
        }
        return (bits[3] << 24) | (bits[2] << 16) | (bits[1] << 8)
               | (bits[0] << 0);
    }();


    // The IBM float is of the form
    // SCCC CCCC QQQQ QQQQ QQQQ QQQQ QQQQ QQQQ
    // where
    //  S is the sign bit
    //  C are the exponent bits
    //  Q are the fraction bits.
    //
    // The IBM number is, assuming the usual binary representation,
    // (-1)^S * 0b0.QQQQ QQQQ QQQQ QQQQ QQQQ QQQQ * 16^(0bCCCCCC - 64)
    //
    // In this function, when we talk about byte "0", we mean the rightmost
    // bit of the above representation, and byte "31" is the leftmost
    // bit.

    // The first 24 bits make up the fraction. If it's zero we can stop,
    // because that is defined as zero.
    const uint32_t frac = (ibm_bits & 0x00FFFFFF);
    if (frac == 0) {
        return {0, 0, 0};
    }

    // Extract the sign of the fraction (bit 31)
    const uint32_t sign = (ibm_bits >> 31) & 0x01;

    // Extract the exponent (bits 24 to 30)
    const uint32_t ibm_exponent_bits = (ibm_bits >> 24) & 0x7F;

    // The IBM exponent is biased by 64, and is a power of 16.
    // We want the unbiased exponent as a power of 2.
    // We use
    //      16^x = 2^(4*x)
    // to convert from base 16 to 2.
    // We also need to use signed types to represent a signed, unbiased
    // exponent.
    const int32_t exp = ((int32_t(ibm_exponent_bits) - 64) * 4);


    // We need to find how much we need to shift frac to have it left justified.
    //
    // In the IBM format, the leading 1 of the fraction is moved as far right as
    // possible. However, since it is using a base 16 exponent, up to 3 leading
    // bits of the fraction can be zero.
    // We therefore, need to shift it left by 8 bits, plus those leading zeros.
    //
    // We use a sneaky trick here: we use the 3 rightmost bits of the IBM
    // fraction as a lookup for a table listing the offsets. Interpreted as an
    // integer, these have the values:
    //  000 -> 0  ---> needs offset of 3
    //  001 -> 1  ---> needs offset of 2
    //  010 -> 2  ---> needs offset of 1
    //  011 -> 3  ---> needs offset of 1
    // and so on.
    //
    const uint32_t offsets[]         = {3, 2, 1, 1, 0, 0, 0, 0};
    const uint32_t leading_frac_bits = frac >> 21;
    const int32_t shift              = offsets[leading_frac_bits];

    const uint32_t shifted_frac = frac << (8 + shift);

    // We need to account for the shift in the exponent as well.
    // We don't include the 8 bit shift, because it's not part of the IBM
    // representation, but the extra `shift` bits are.
    const int32_t shifted_exp = exp - shift;

    return {sign, shifted_exp, shifted_frac};
}


/// Right shift the value \c value \c shift places to the right
/// (i.e. `\c value >> \c shift`, or divide by 2**(\c shift)), applying
/// round-half-even rounding to the result. This is intended to emulate casting
/// from double to float.
/// @param[in] value The value to right shift.
/// @param[in] shift The amount to right shift by.
/// @returns The result of \c value >> \c shift, with round-half-even applied.
static uint32_t rshift_with_rounding(uint32_t value, uint32_t shift) {

    // Get the part of the `value` that will be truncated. This will be the
    // last `shift` bits

    // mask is 0000 0000 ... 1111, where the 1s are `shift` bits wide
    const uint32_t mask = (1UL << shift) - 1;

    // trunc is the last `shift` bits of `value`
    const uint32_t trunc = value & mask;

    // Shift and truncate `value`.
    uint32_t truncated_value = value >> shift;

    // Apply rounding

    // We use round-half-even rounding, as that's the most common rounding
    // implementation for IEEE numbers.
    //
    // In round-half-even rounding, if a number is exactly 0bxxxx.1000...,
    // i.e. exactly half-way between two integer values, it is always
    // rounded to the nearest even integer.
    //
    // The following is a list of all the potential roundings:
    //      0bxxx0.0...1 = QQQ0
    //      0bxxx0.10    = QQQ0
    //      0bxxx0.1...1 = QQQ1
    //      0bxxx1.10    = QQQ1 + 0001
    //      0bxxx1.0...1 = QQQ1
    //      0bxxx1.1...1 = QQQ1


    // half is the value 0000...1000... representing when the truncated
    // part is exactly half, i.e. 0b0.1000...
    const uint32_t half = (1UL << (shift-1));

    // If the truncated part is > 0b0.1000, round up. When it's < 0b0.1000,
    // round down, i.e. just allow the number to be truncated.
    if(trunc > half) {
        truncated_value += 1;
    }
    else if(trunc == half) {
        // If the truncated part is exactly half, and value is odd, round
        // up, i.e. round to the nearest even.
        // If it's even, just allow it to remain truncated.
        if((truncated_value & 0x01) == 0x01) {
            truncated_value += 1;
        }
    }

    return truncated_value;
}


uint32_t to_IEEE(FloatComponents components)
{
    uint32_t sign = components.sign;
    int32_t exp   = components.exponent;
    uint32_t frac = components.significand;


    // An IEEE number is of the form
    // SCCC CCCC CQQQ QQQQ QQQQ QQQQ QQQQ QQQQ
    // where
    //  S is the sign bit
    //  C are the exponent bits
    //  Q are the fraction bits.
    //
    // The IEEE number is, assuming the usual binary representation,
    // (-1)^S * 0b1.QQQ QQQQ QQQQ QQQQ QQQQ QQQQ * 2^(0bCCC CCCC C - 127)
    //
    // We move the right-justified 1 in frac right 8 bits to position 21.
    // It will be dropped in normal numbers, and used in denormal numbers.
    //
    frac = frac >> 8;

    // Now we apply the IEEE bias to the exponent.
    // The IEEE bias is 127. We subtract an extra 1 to account for the implicit
    // 1 in the significand.
    exp = exp + 127 - 1;


    // Denormalized numbers
    //
    // In IEEE, denormalized numbers are values of 2^-126 or smaller.
    // These numbers are of the form
    // S000 0000 0QQQ QQQQ QQQQ QQQQ QQQQ
    // where
    //  S is the sign bit
    //  Q are the fraction bits.
    //
    // The denormalized IEEE number is
    // (-1)^S * 0b0.QQQ QQQQ QQQQ QQQQ QQQQ * 2^-126
    //
    // Note here the initial digit is 0.QQQ, rather than 1.QQQ, as was the
    // case with regular IEEE numbers.
    // Converting from normal IEEE to denormalized IEEE, then, requires an
    // extra right shift, and an explicit leading 1 in the significand.
    //
    // We know we need a denormalized number if our exponent is negative after
    // biasing. In this case, the negative value represents the right shift
    // needed to represent the number.
    //
    // Note: this will catch +/- 0 as well, but the frac is 0, so this shift
    //       will make no difference.
    //
    if (exp <= 0) {
        // denorm numbers get an extra shift of 1, because they're in the
        // form 0.bbbb... rather than 1.bbbb... for regular numbers.

        frac = rshift_with_rounding(frac, -exp+1);
        exp  = 0;
    }

    // Overflow
    //
    // If the exponent is larger than an IEEE representable number, we return
    // +- inf.
    // This number is in the form
    // S111 1111 1000 0000 0000 0000 0000 0000
    //
    if (exp >= 0xFF) {
        frac = 0;
        exp  = 0xFF;
    }

    // Drop bit 24 of frac
    frac = frac & 0x7FFFFF;

    // Make sure we've only got 8 bits for the exponent!
    exp = exp & 0xFF;

    // Shift components to the positions of IEEE float.
    // Frac is already at the correct position.

    // Sign is at bit 32
    sign <<= 31;

    // Exp is at bits 31 to 24
    exp <<= 23;

    return sign | exp | frac;
}


// TODO: Haven't done anything for underflow, overflow or nans
// TODO: Not extensively tested yet
float convertIBMtoIEEE(const float f, bool bigEndian)
{
    const uint32_t i          = toint(f);
    const auto ibm_components = from_IBM(i, bigEndian);
    const uint32_t ieee       = to_IEEE(ibm_components);

    return tofloat(ieee);
}

}  // namespace PIOL
