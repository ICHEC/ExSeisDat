#include "exseisdat/test/Index_hash.hh"

#include "exseisdat/test/catch2.hh"

#include <cstdint>
#include <limits>
#include <random>
#include <type_traits>

TEST_CASE("Index_hash", "[Index_hash][test]")
{
    using exseis::test::Index_hash;

    SECTION ("Index_hash::get(i) is repeatable") {
        std::map<uint64_t, uint64_t> hashed_values;

        // Corner cases
        constexpr uint64_t uint64_max = std::numeric_limits<uint64_t>::max();

        hashed_values[0]          = Index_hash::get(0);
        hashed_values[uint64_max] = Index_hash::get(uint64_max);

        REQUIRE(hashed_values[0] == Index_hash::get(0));
        REQUIRE(hashed_values[uint64_max] == Index_hash::get(uint64_max));

        // Generate some random values
        constexpr uint64_t seed = 0;
        std::mt19937_64 random(seed);

        constexpr size_t number_of_values = 1024;
        for (size_t i = 0; i < number_of_values; i++) {
            const auto index     = random();
            hashed_values[index] = Index_hash::get(index);
        }

        // Check the values match again
        for (const auto& kv : hashed_values) {
            const auto key   = kv.first;
            const auto value = kv.second;

            REQUIRE(Index_hash::get(key) == value);
        }
    }

    SECTION ("Index_hash::get(i) is constexpr") {
        constexpr auto v_0 = Index_hash::get(0);
        REQUIRE(v_0 == Index_hash::get(0));
        STATIC_REQUIRE(v_0 == Index_hash::get(0));
    }

    SECTION ("Index_hash h; h(i)") {
        Index_hash h;
        constexpr Index_hash ch;

        constexpr uint64_t seed = 1;
        std::mt19937_64 random(seed);

        constexpr size_t number_of_values = 1024;
        for (size_t i = 0; i < number_of_values; i++) {
            REQUIRE(h(i) == Index_hash::get(i));
            REQUIRE(ch(i) == Index_hash::get(i));

            REQUIRE(h.get(i) == Index_hash::get(i));
            REQUIRE(ch.get(i) == Index_hash::get(i));
        }
    }

    SECTION ("Index_hash::get<T>(i)") {
        constexpr uint64_t seed = 2;
        std::mt19937_64 random(seed);

        constexpr size_t number_of_values = 1024;
        for (size_t i = 0; i < number_of_values; i++) {
            const auto index = random();

            // Check get<T> returns the correct type
            STATIC_REQUIRE(std::is_same<
                           decltype(Index_hash::get(index)), uint64_t>::value);
            STATIC_REQUIRE(
                std::is_same<
                    decltype(Index_hash::get<int32_t>(index)), int32_t>::value);
            STATIC_REQUIRE(std::is_same<
                           decltype(Index_hash::get<unsigned char>(index)),
                           unsigned char>::value);

            // Generate hash for a few types
            const auto i_default       = Index_hash::get(index);
            const auto i_int32         = Index_hash::get<int32_t>(index);
            const auto i_unsigned_char = Index_hash::get<unsigned char>(index);

            // Sanity check type sizes
            STATIC_REQUIRE(sizeof(i_default) == 8);
            STATIC_REQUIRE(sizeof(i_int32) == 4);
            STATIC_REQUIRE(sizeof(i_unsigned_char) == 1);

            // Bitwise cast unsigned to signed, compare truncated values
            uint32_t i_int32_uint32;
            std::memcpy(&i_int32_uint32, &i_int32, sizeof(int32_t));
            REQUIRE(i_int32_uint32 == (i_default & 0xFFFFFFFFu));

            REQUIRE(i_unsigned_char == (i_default & 0xFFu));
        }
    }
}
