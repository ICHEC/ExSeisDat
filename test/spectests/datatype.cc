#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "ExSeisDat/utils/encoding/number_encoding.hh"

#include <bitset>
#include <cmath>
#include <cstring>
#include <stdint.h>
#include <vector>

using namespace exseis::utils;

TEST(Datatype, getHost32Bit1)
{
    std::vector<unsigned char> src = {1, 1, 1, 1};
    auto h                         = getHost<int32_t>(src.data());
    EXPECT_EQ(16843009, h);
}

TEST(Datatype, getHost32Bit2)
{
    std::vector<unsigned char> src = {0x78, 0x9A, 0xBC, 0xDE};
    auto h                         = getHost<int32_t>(src.data());
    EXPECT_EQ(2023406814, h);
}

TEST(Datatype, getHost32Bit3)
{
    std::vector<unsigned char> src = {0x88, 0x9A, 0xBC, 0xDE};
    auto h                         = getHost<int32_t>(src.data());

    int32_t ans = 0x889ABCDE;
    EXPECT_EQ(ans, h);
}

std::string printBinary(uint32_t val)
{
    std::stringstream s;
    for (int i = 31; i >= 0; i--) {
        if (!((i + 1) % 4) && i != 31) s << " ";
        s << ((val >> i) & 0x1);
    }
    return s.str();
}


struct IBM_native_pair {
    std::bitset<32> ibm;
    std::bitset<32> native;
    long double value;

    IBM_native_pair(const std::bitset<32> ibm, const std::bitset<32> native) :
        ibm(ibm),
        native(native)
    {
    }
};

IBM_native_pair make_IBM_native_pair(
  int sign, int8_t exponent, uint32_t significand)
{
    // Make sure the input values are in the right range
    assert((sign == -1 || sign == 1) && "Sign must be -1 or 1.");

    // IBM supports exponent from -64 to 63.
    assert(
      exponent >= -64 && exponent <= 63
      && "Exponent must be in range [-64, 63].");

    // The IBM significand is 24 bits long.
    assert(significand <= 0xFFFFFF && "Significand must be less than 2^25.");

    // IBM expects at most 3 zeros at the start of the significand.
    assert(significand > 0x0FFFFF && "Significand must be greater than 2^20.");

    // Some tests for bit twiddling
    static_assert(sizeof(float) == 4, "Float isn't 4 bytes!");


    // Initialize the first byte. The sign and exponent will be added to this.
    std::bitset<32> ibm_byte_1 = 0x00000000;

    // Set sign bit
    // The highest bit is 1 for negative numbers.
    if (sign == -1) {
        ibm_byte_1.set(7);
    }

    // Set exponent bits
    // The exponent is 7 bits, biased by 64.
    // So
    //  0b0000000 = 0   represents the exponent of -64,
    //  0b1000000 = 64  represents the exponent of   0, and
    //  0b1111111 = 127 represents the exponent of +63
    assert((exponent + 64) >= 0 && "Biased exponent must be positive!");
    assert(
      (exponent + 64) <= 127 && "Biased exponent must be in range [0, 127].");

    // Assuming the native unsigned integer representation is standard binary,
    // we add the bias, convert the signed value to unsigned, and get the bits.
    const std::bitset<32> biased_exponent = static_cast<uint8_t>(exponent + 64);

    // Add the exponent to the first byte
    ibm_byte_1 |= biased_exponent;

    // A function for getting the lowest 8 bits / byte from the bitset
    auto get_lowest_byte = [](std::bitset<32> b) {
        return b & std::bitset<32>(0xFF);
    };

    // IBM byte 2, 3, and 4 are the significand in big endian, with the bytes
    // left-justified.
    // Again, assuming the native unsigned integer representation is standard
    // binary.
    const std::bitset<32> ibm_byte_2 = get_lowest_byte(significand >> 16);
    const std::bitset<32> ibm_byte_3 = get_lowest_byte(significand >> 8);
    const std::bitset<32> ibm_byte_4 = get_lowest_byte(significand >> 0);

    // Put all the bits together.
    const std::bitset<32> ibm_bits =
      ibm_byte_1 << 24 | ibm_byte_2 << 16 | ibm_byte_3 << 8 | ibm_byte_4 << 0;

    // Construct the equivalent value using the native floating point number.
    // In these tests, we assume this is IEEE 754 binary32 with rounding for
    // any truncated bits.
    const float native_value =
      sign * static_cast<long double>(significand)
      * std::pow(2.0L, static_cast<long double>(-24 + 4 * exponent));

    // Convert the native rep directly into an unsigned type.
    uint32_t native_rep = 0;
    std::memcpy(&native_rep, &native_value, sizeof(uint32_t));

    // Assuming the endianness of the float and the unsigned integer are the
    // same, we expect std::bitset to handle any necessary byte swapping for
    // a consistent representation.
    const std::bitset<32> native_bits = native_rep;


    return {ibm_bits, native_bits};
}


TEST(Datatype, IBMToIEEE)
{
    // Test constructing IBM and floating point values for a range of signs,
    // exponents and significand.

    for (int sign : {1, -1}) {
        for (int8_t exponent = -32; exponent < 32; exponent += 7) {
            for (int32_t base_significand = 1; base_significand < 0xF00000;
                 base_significand += 7) {

                // Add the minimum allowed significand value to the base
                // significand.
                // Note, this doesn't fully explore denormalised values.
                const uint32_t minimum_allowed_significand = 0x0FFFFF;
                const uint32_t significand =
                  base_significand + minimum_allowed_significand;

                // Build an equivalent pair of IBM and native floating point
                // values
                auto pair = make_IBM_native_pair(sign, exponent, significand);

                // Integer representations for the IBM and native float
                const uint32_t built_ibm   = pair.ibm.to_ulong();
                const uint32_t built_float = pair.native.to_ulong();

                // Built an ieee float with convertIBMtoIEEE using the IBM
                // float we just made.
                float ibm_float = 0;
                std::memcpy(&ibm_float, &built_ibm, sizeof(float));
                const float ieee = convertIBMtoIEEE(ibm_float, false);

                // Integer representation for the IEEE constructed float.
                uint32_t built_ieee = 0;
                std::memcpy(&built_ieee, &ieee, sizeof(uint32_t));

                // Test the IEEE and natively calculated values are the same
                // for the IBM float.
                auto to_float = [](uint32_t i) {
                    float f = 0;
                    std::memcpy(&f, &i, sizeof(float));
                    return f;
                };

                // Test exact equality for normal values
                ASSERT_EQ(built_float, built_ieee)
                  << std::endl
                  << "sign: " << sign << std::endl
                  << "exponent: " << static_cast<int>(exponent) << std::endl
                  << "significand: " << printBinary(significand) << " , "
                  << std::hex << significand << std::endl
                  << std::endl
                  << "IBM: " << std::endl
                  << "SCCC CCCC QQQQ QQQQ QQQQ QQQQ QQQQ QQQQ" << std::endl
                  << printBinary(built_ibm) << std::endl
                  << std::endl
                  << "Native: " << std::hexfloat << to_float(built_float)
                  << std::endl
                  << "SCCC CCCC CQQQ QQQQ QQQQ QQQQ QQQQ QQQQ" << std::endl
                  << printBinary(built_float) << std::endl
                  << std::endl
                  << "IEEE:   " << std::hexfloat << to_float(built_ieee)
                  << std::endl
                  << "SCCC CCCC CQQQ QQQQ QQQQ QQQQ QQQQ QQQQ" << std::endl
                  << printBinary(built_ieee) << std::endl;
            }
        }
    }
}
