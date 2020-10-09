#include "exseisdat/utils/encoding/number_encoding.hh"

#include "exseisdat/test/IBM_number.hh"
#include "exseisdat/test/catch2.hh"

#include <bitset>
#include <cmath>
#include <cstring>
#include <stdint.h>
#include <vector>

using namespace exseis::utils;

// namespace {
//
// std::string print_binary(uint32_t val)
// {
//     std::string s;
//     s.reserve(32 + 7);
//     for (int i = 31; i >= 0; i--) {
//         if ((i + 1) % 4 == 0 && i != 31) {
//             s.push_back(' ');
//         }
//         s.push_back(((val >> i) & 0x1) == 0 ? '0' : '1');
//     }
//     assert(s.size() == (32 + 7));
//     return s;
// }
//
// }  // namespace


TEST_CASE("from_big_endian", "[number_encoding]")
{
    const uint8_t a = GENERATE(take(10, random(0x00u, 0xFFu)));
    const uint8_t b = GENERATE(take(10, random(0x00u, 0xFFu)));
    const uint8_t c = GENERATE(take(10, random(0x00u, 0xFFu)));
    const uint8_t d = GENERATE(take(10, random(0x00u, 0xFFu)));

    const uint32_t native_uint32 =
        (uint32_t(a) << (8 * 3)) + (uint32_t(b) << (8 * 2))
        + (uint32_t(c) << (8 * 1)) + (uint32_t(d) << (8 * 0));

    // Check conversion to uint32_t
    REQUIRE(from_big_endian<uint32_t>(a, b, c, d) == native_uint32);


    // Check conversion to int32_t
    int32_t native_int32 = 0;
    std::memcpy(&native_int32, &native_uint32, sizeof(uint32_t));

    REQUIRE(from_big_endian<int32_t>(a, b, c, d) == native_int32);


    // Check conversion to float
    // Careful of float comparison with '=', the compiler complains
    const float float_from_be     = from_big_endian<float>(a, b, c, d);
    uint32_t float_from_be_uint32 = 0;
    std::memcpy(&float_from_be_uint32, &float_from_be, sizeof(uint32_t));

    REQUIRE(float_from_be_uint32 == native_uint32);
}


TEST_CASE("from_ibm_to_float", "[number_encoding]")
{

    // Test constructing IBM and floating point values for a range of signs,
    // exponents and significand.

    // We could test every single value, but it takes a little too long
    // to sit through.

    // Note: this doesn't fully explore denormalised values.

    // To find the range of values that can be expressed in both
    // IEEE and IBM formats:
    //
    // IEEE exponent: 2^exponent @ 8 bits
    //  => IEEE exponent min: 2^-127
    // IBM exponent:  16^exponent = 2^(4*exponent) @ 7 bits
    //  => IBM exponent min: 2^-4*64 = 2^-256
    // Therefore, min IBM exponent representable in IEEE
    // (not accounting for significand shifting)
    //  => exponent = -32
    //  => 2^-4*32 = 2^-128
    //
    // Similarly, max IBM exponent representable in IEEE
    //  => exponent = +31
    //  => 2^4*31 = 2^124
    //
    // Since the significand can shift by up to 3 places, the minumum
    // is really
    //  => exponent = -29
    // However, because of denormal floating-point representation,
    // -32 will work anyway. A surprisingly low number for
    // the significand will actually work due to denormalization!

    int sign        = GENERATE(1, -1);
    int8_t exponent = GENERATE(range(-32, 31, 7), 0, 31);
    // Loop instead of GENERATE for performance
    for (uint32_t significand = 0x100000U; significand <= 0xFFFFFFU;
         significand += 7) {

        // Build an equivalent pair of IBM and native floating point
        // values
        auto ibm_float = exseis::test::IBM_number(sign, exponent, significand);

        // Integer representations for the IBM and native float
        const auto ibm_float_be      = ibm_float.binary();
        const float ibm_float_native = ibm_float.native();

        const float ieee = from_ibm_to_float(ibm_float_be, true);

        // Integer representation for the IEEE constructed float and IBM_number
        // native float.
        uint32_t ieee_bits = 0;
        std::memcpy(&ieee_bits, &ieee, sizeof(uint32_t));

        uint32_t ibm_float_native_bits = 0;
        std::memcpy(
            &ibm_float_native_bits, &ibm_float_native, sizeof(uint32_t));

        // Test exact equality for normal values
        REQUIRE(ibm_float_native_bits == ieee_bits);

        // // Test the IEEE and natively calculated values are the same
        // // for the IBM float.
        // auto to_float = [](uint32_t i) {
        //     float f = 0;
        //     std::memcpy(&f, &i, sizeof(float));
        //     return f;
        // };
        //
        // std::cout
        //     << std::endl
        //     << "sign: " << sign << std::endl
        //     << "exponent: " << static_cast<int>(exponent) << std::endl
        //     << "significand: " << print_binary(significand) << " , "
        //     << std::hex << significand << std::endl
        //     << std::endl
        //     << "IBM: " << std::endl
        //     << "SCCC CCCC QQQQ QQQQ QQQQ QQQQ QQQQ QQQQ" << std::endl
        //     << print_binary(built_ibm) << std::endl
        //     << std::endl
        //     << "Native: " << std::hexfloat << to_float(built_float)
        //     << std::endl
        //     << "SCCC CCCC CQQQ QQQQ QQQQ QQQQ QQQQ QQQQ" << std::endl
        //     << print_binary(built_float) << std::endl
        //     << std::endl
        //     << "IEEE:   " << std::hexfloat << to_float(built_ieee)
        //     << std::endl
        //     << "SCCC CCCC CQQQ QQQQ QQQQ QQQQ QQQQ QQQQ" << std::endl
        //     << print_binary(built_ieee) << std::endl;
    }
}
