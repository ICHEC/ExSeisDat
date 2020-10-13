#ifndef EXSEIS_TEST_CATCH_HH
#define EXSEIS_TEST_CATCH_HH

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch2/catch.hpp"

#include <array>

// NOLINTNEXTLINE
namespace Catch {
template<>
struct StringMaker<std::array<unsigned char, 4>> {
    static std::string convert(std::array<unsigned char, 4> const& value)
    {
        static constexpr char hex_values[] = {'0', '1', '2', '3', '4', '5',
                                              '6', '7', '8', '9', 'A', 'B',
                                              'C', 'D', 'E', 'F'};

        char buf[27] = "{ 0xHH, 0xHH, 0xHH, 0xHH }";
        //  ^ offset=4  ^^^^^^ stride = 6

        static constexpr size_t offset = 4;
        static constexpr size_t stride = 6;

        buf[offset + 0 * stride + 0] = hex_values[(value[0] >> 4) & 0x0F];
        buf[offset + 0 * stride + 1] = hex_values[(value[0] >> 0) & 0x0F];

        buf[offset + 1 * stride + 0] = hex_values[(value[1] >> 4) & 0x0F];
        buf[offset + 1 * stride + 1] = hex_values[(value[1] >> 0) & 0x0F];

        buf[offset + 2 * stride + 0] = hex_values[(value[2] >> 4) & 0x0F];
        buf[offset + 2 * stride + 1] = hex_values[(value[2] >> 0) & 0x0F];

        buf[offset + 3 * stride + 0] = hex_values[(value[3] >> 4) & 0x0F];
        buf[offset + 3 * stride + 1] = hex_values[(value[3] >> 0) & 0x0F];

        return buf;
    }
};

template<>
struct StringMaker<std::array<unsigned char, 8>> {
    static std::string convert(std::array<unsigned char, 8> const& value)
    {
        static constexpr char hex_values[] = {'0', '1', '2', '3', '4', '5',
                                              '6', '7', '8', '9', 'A', 'B',
                                              'C', 'D', 'E', 'F'};

        char buf[51] = "{ 0xHH, 0xHH, 0xHH, 0xHH, 0xHH, 0xHH, 0xHH, 0xHH }";
        //  ^ offset=4  ^^^^^^ stride = 6

        static constexpr size_t offset = 4;
        static constexpr size_t stride = 6;

        buf[offset + 0 * stride + 0] = hex_values[(value[0] >> 4) & 0x0F];
        buf[offset + 0 * stride + 1] = hex_values[(value[0] >> 0) & 0x0F];

        buf[offset + 1 * stride + 0] = hex_values[(value[1] >> 4) & 0x0F];
        buf[offset + 1 * stride + 1] = hex_values[(value[1] >> 0) & 0x0F];

        buf[offset + 2 * stride + 0] = hex_values[(value[2] >> 4) & 0x0F];
        buf[offset + 2 * stride + 1] = hex_values[(value[2] >> 0) & 0x0F];

        buf[offset + 3 * stride + 0] = hex_values[(value[3] >> 4) & 0x0F];
        buf[offset + 3 * stride + 1] = hex_values[(value[3] >> 0) & 0x0F];

        buf[offset + 4 * stride + 0] = hex_values[(value[4] >> 4) & 0x0F];
        buf[offset + 4 * stride + 1] = hex_values[(value[4] >> 0) & 0x0F];

        buf[offset + 5 * stride + 0] = hex_values[(value[5] >> 4) & 0x0F];
        buf[offset + 5 * stride + 1] = hex_values[(value[5] >> 0) & 0x0F];

        buf[offset + 6 * stride + 0] = hex_values[(value[6] >> 4) & 0x0F];
        buf[offset + 6 * stride + 1] = hex_values[(value[6] >> 0) & 0x0F];

        buf[offset + 7 * stride + 0] = hex_values[(value[7] >> 4) & 0x0F];
        buf[offset + 7 * stride + 1] = hex_values[(value[7] >> 0) & 0x0F];

        return buf;
    }
};
}  // namespace Catch

#endif  // EXSEIS_TEST_CATCH_HH
