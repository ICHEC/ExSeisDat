#ifndef EXSEISDAT_TEST_IBM_NUMBER_HH
#define EXSEISDAT_TEST_IBM_NUMBER_HH

#include "exseisdat/utils/encoding/number_encoding.hh"

#include <cmath>
#include <cstdint>
#include <cstring>

namespace exseis {
namespace test {

class IBM_number {
    using Binary = std::array<unsigned char, 4>;
    using Native = float;

    Binary m_ibm{};
    Native m_native = 0;

  public:
    Binary binary() const { return m_ibm; }
    Native native() const { return m_native; }

    IBM_number(int sign, int8_t exponent, uint32_t significand)
    {

        // Make sure the input values are in the right range
        assert((sign == -1 || sign == 1) && "Sign must be -1 or 1.");

        // IBM supports exponent from -64 to 63.
        assert(
            exponent >= -64 && exponent <= 63
            && "Exponent must be in range [-64, 63].");

        // The IBM significand is 24 bits long.
        assert(
            significand <= 0xFFFFFF && "Significand must be less than 2^25.");

        // IBM expects at most 3 zeros at the start of the significand.
        assert(
            significand > 0x0FFFFF && "Significand must be greater than 2^20.");

        // Some tests for bit twiddling
        static_assert(sizeof(float) == 4, "Float isn't 4 bytes!");


        // Initialize the first byte. The sign and exponent will be added to
        // this.
        uint8_t ibm_byte_1 = 0;

        // Set sign bit
        // The highest bit is 1 for negative numbers.
        if (sign == -1) {
            ibm_byte_1 += 0x80;
        }

        // Set exponent bits
        // The exponent is 7 bits, biased by 64.
        // So
        //  0b0000000 = 0   represents the exponent of -64,
        //  0b1000000 = 64  represents the exponent of   0, and
        //  0b1111111 = 127 represents the exponent of +63
        assert((exponent + 64) >= 0 && "Biased exponent must be positive!");
        assert(
            (exponent + 64) <= 127
            && "Biased exponent must be in range [0, 127].");

        // Assuming the native unsigned integer representation is standard
        // binary, we add the bias, convert the signed value to unsigned, and
        // get the bits.
        const uint8_t biased_exponent = static_cast<uint8_t>(exponent + 64);

        // Add the exponent to the first byte
        ibm_byte_1 |= biased_exponent;

        // IBM byte 2, 3, and 4 are the significand in big endian, with the
        // bytes left-justified. Again, assuming the native unsigned integer
        // representation is standard binary rep
        m_ibm[0] = ibm_byte_1;
        m_ibm[1] = (significand >> (2 * 8)) & 0xFFU;
        m_ibm[2] = (significand >> (1 * 8)) & 0xFFU;
        m_ibm[3] = (significand >> (0 * 8)) & 0xFFU;


        // Construct the equivalent value using the native floating point
        // number. In these tests, we assume this is IEEE 754 binary32 with
        // rounding for any truncated bits.
        m_native =
            sign * static_cast<long double>(significand)
            * std::pow(2.0L, static_cast<long double>(-24 + 4 * exponent));
    }
};

}  // namespace test
}  // namespace exseis

#endif  // EXSEISDAT_TEST_IBM_NUMBER_HH
