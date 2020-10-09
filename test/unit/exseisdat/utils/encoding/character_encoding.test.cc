#include "exseisdat/utils/encoding/character_encoding.hh"

#include "exseisdat/test/catch2.hh"

namespace {

constexpr const char ascii[] =
    "This is a string for testing EBCDIC conversion etc.\n"
    "The quick brown fox jumps over the lazy dog."
    " !\"#$%&'()*+,-./"
    "0123456789:;<=>?"
    "@ABCDEFGHIJKLMNO"
    "PQRSTUVWXYZ[\\]^_"
    "`abcdefghijklmno"
    "pqrstuvwxyz{|}~"
    "\a\b\t\n\v\f\r\x1b";  // \0 (implicit)

// The test_string in EBCDIC encoding.
constexpr const unsigned char ebcdic[] = {
    // This is a string for testing EBCDIC conversion etc.\n
    0xE3, 0x88, 0x89, 0xA2, 0x40, 0x89, 0xA2, 0x40, 0x81, 0x40, 0xA2, 0xA3,
    0x99, 0x89, 0x95, 0x87, 0x40, 0x86, 0x96, 0x99, 0x40, 0xA3, 0x85, 0xA2,
    0xA3, 0x89, 0x95, 0x87, 0x40, 0xC5, 0xC2, 0xC3, 0xC4, 0xC9, 0xC3, 0x40,
    0x83, 0x96, 0x95, 0xA5, 0x85, 0x99, 0xA2, 0x89, 0x96, 0x95, 0x40, 0x85,
    0xA3, 0x83, 0x4B, 0x25,

    // The quick brown fox jumps over the lazy dog.
    0xE3, 0x88, 0x85, 0x40, 0x98, 0xA4, 0x89, 0x83, 0x92, 0x40, 0x82, 0x99,
    0x96, 0xA6, 0x95, 0x40, 0x86, 0x96, 0xA7, 0x40, 0x91, 0xA4, 0x94, 0x97,
    0xA2, 0x40, 0x96, 0xA5, 0x85, 0x99, 0x40, 0xA3, 0x88, 0x85, 0x40, 0x93,
    0x81, 0xA9, 0xA8, 0x40, 0x84, 0x96, 0x87, 0x4B,

    // [[space]]!"#$%&'()*+,-./
    0x40, 0x5a, 0x7f, 0x7b, 0x5b, 0x6c, 0x50, 0x7d, 0x4d, 0x5d, 0x5c, 0x4e,
    0x6b, 0x60, 0x4b, 0x61,

    // 0123456789:;<=>?
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0x7a, 0x5e,
    0x4c, 0x7e, 0x6e, 0x6f,

    // @ABCDEFGHIJKLMNO
    0x7c, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xd1, 0xd2,
    0xd3, 0xd4, 0xd5, 0xd6,

    // PQRSTUVWXYZ[\]^_
    0xd7, 0xd8, 0xd9, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xad,
    0xe0, 0xbd, 0x9a, 0x6d,

    // `abcdefghijklmno
    0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91, 0x92,
    0x93, 0x94, 0x95, 0x96,

    // pqrstuvwxyz{|}~
    0x97, 0x98, 0x99, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xc0,
    0x4f, 0xd0, 0xa1,

    // \a\b\t\n\v\f\r\x1b
    0x2f, 0x16, 0x05, 0x25, 0x0b, 0x0c, 0x0d, 0x27,

    // \0
    0x00};

static_assert(
    (std::cend(ascii) - std::cbegin(ascii))
        == (std::cend(ebcdic) - std::cbegin(ebcdic)),
    "ASCII and EBCDIC strings must be the same length");

}  // namespace

TEST_CASE("Character Encoding", "[utils][EBCIDIC][ASCII]")
{
    using namespace exseis::utils::character_encoding;

    // Expect strings to be null terminated
    STATIC_REQUIRE(*(std::end(ascii) - 1) == '\0');
    STATIC_REQUIRE(*(std::end(ebcdic) - 1) == '\0');

    // Expect strings to be the same length
    STATIC_REQUIRE(
        (std::cend(ascii) - std::cbegin(ascii))
        == (std::cend(ebcdic) - std::cbegin(ebcdic)));

    SECTION ("Translate between test ASCII and EBCDIC strings") {
        const size_t strlen = std::end(ascii) - std::begin(ascii);

        for (size_t i = 0; i < strlen; i++) {
            const char ascii_c           = ascii[i];
            const unsigned char ebcdic_c = ebcdic[i];

            REQUIRE(ebcdic_c == to_ebcdic_from_ascii(ascii_c));
            REQUIRE(ascii_c == to_ascii_from_ebcdic(ebcdic_c));
        }
    }

    SECTION ("is_printable_ascii") {
        // Loop over 8-bit ASCII
        for (size_t i = 0; i < 256; i++) {
            const char ascii_c = static_cast<char>(i);

            // Excluding DEL as printable
            if (i >= 32 && i <= 126) {
                REQUIRE(is_printable_ascii(ascii_c) == true);
            }
            else {
                REQUIRE(is_printable_ascii(ascii_c) == false);
            }
        }
    }

    SECTION ("is_printable_ebcdic <-> is_printable_ascii") {
        // Loop over 8-bit ASCII
        for (size_t i = 0; i < 256; i++) {
            const char ascii_c           = static_cast<char>(i);
            const unsigned char ebcdic_c = to_ebcdic_from_ascii(ascii_c);

            REQUIRE(
                is_printable_ascii(ascii_c) == is_printable_ebcdic(ebcdic_c));
        }
    }

    SECTION ("ASCII <-> EBCDIC conversion consistency") {
        // Loop over 8-bit ASCII values
        for (size_t i = 0; i < 256; i++) {
            const char ascii_c = static_cast<char>(i);
            const char round_trip_ascii_c =
                to_ascii_from_ebcdic(to_ebcdic_from_ascii(ascii_c));

            const unsigned char ebcdic_c = static_cast<unsigned char>(i);
            const unsigned char round_trip_ebcdic_c =
                to_ebcdic_from_ascii(to_ascii_from_ebcdic(ebcdic_c));

            const char ascii_sub           = 0x1A;
            const unsigned char ebcdic_sub = 0x3FU;

            // NULL should map to NULL
            if (i == 0) {
                // ASCII(NULL) == EBCDIC(NULL)
                REQUIRE(ascii_c == 0);
                REQUIRE(to_ebcdic_from_ascii(ascii_c) == 0);
                REQUIRE(ebcdic_c == 0);
                REQUIRE(to_ascii_from_ebcdic(ebcdic_c) == 0);

                REQUIRE(ascii_c == round_trip_ascii_c);
                REQUIRE(ebcdic_c == round_trip_ebcdic_c);
            }

            // No other character should map to NULL
            // (Checking there are no "empty" mappings")
            if (i != 0) {
                REQUIRE(ascii_c != 0);
                REQUIRE(to_ebcdic_from_ascii(ascii_c) != 0);
                REQUIRE(ebcdic_c != 0);
                REQUIRE(to_ascii_from_ebcdic(ebcdic_c) != 0);
            }

            // Printable characters should survive a round-trip
            if (is_printable_ascii(ascii_c)) {
                REQUIRE(ascii_c == round_trip_ascii_c);
            }
            if (is_printable_ebcdic(ebcdic_c)) {
                REQUIRE(ebcdic_c == round_trip_ebcdic_c);
            }

            // Anything that maps forward should map back
            if (to_ebcdic_from_ascii(ascii_c) != ebcdic_sub) {
                REQUIRE(ascii_c == round_trip_ascii_c);
            }
            if (to_ascii_from_ebcdic(ebcdic_c) != ascii_sub) {
                REQUIRE(ebcdic_c == round_trip_ebcdic_c);
            }
        }
    }
}
