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

        buf[4] = hex_values[(value[0] >> 4) & 0x0F];
        buf[5] = hex_values[(value[0] >> 0) & 0x0F];

        buf[10] = hex_values[(value[1] >> 4) & 0x0F];
        buf[11] = hex_values[(value[1] >> 0) & 0x0F];

        buf[16] = hex_values[(value[2] >> 4) & 0x0F];
        buf[17] = hex_values[(value[2] >> 0) & 0x0F];

        buf[22] = hex_values[(value[3] >> 4) & 0x0F];
        buf[23] = hex_values[(value[3] >> 0) & 0x0F];

        return buf;
    }
};
}  // namespace Catch

#endif  // EXSEIS_TEST_CATCH_HH
