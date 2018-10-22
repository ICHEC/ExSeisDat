////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   Functions for converting between ASCII and EBCDIC encoding.
/// @details We define a list of ASCII / EBCDIC pairs of characters.  We use
///          this list to create a map from ASCII characters to the equivalent
///          EBCDIC character, and the reverse map, EBCDIC to ASCII.
///          We then provide functions for these map lookups.
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/utils/encoding/character_encoding.hh"

#include <algorithm>
#include <array>


namespace exseis {
namespace utils {
inline namespace character_encoding {


/// @brief Implementation details for the character_encoding functions.
///
/// This namespace defines static functions for building the maps which we will
/// use to statically initialize the EBCDIC -> ASCII and ASCII -> EBCDIC maps.
/// The maps are defined as lists sorted by the key value (i.e. by ASCII value
/// or by EBCDIC value), and lookup is done using a sorted list lookup.
/// This is used for the implementation of to_ascii_from_ebcdic and
/// to_ebcdic_from_ascii.
///
namespace {

/// A structure to represent a character in both ASCII and EBCDIC.
struct Character_encoding {

    /// Construct an ASCII / EBCDIC equivalent character
    /// @param[in] ascii  The ASCII char code
    /// @param[in] ebcdic The EBCDIC char code
    explicit Character_encoding(unsigned char ascii, unsigned char ebcdic) :
        ascii(ascii),
        ebcdic(ebcdic)
    {
    }

    /// The ASCII character
    unsigned char ascii;

    /// The EBCDIC character
    unsigned char ebcdic;
};

/// @brief The SUB character.
///
/// Used when a character is not representable in the given encoding.
static const Character_encoding char_sub{0x1Au, 0x3Fu};

/// The array type for all the ASCII/EBCDIC pairs.
using Character_encodings = std::array<Character_encoding, 126>;

/// @brief A list of ASCII / EBCDIC pairs.
///
/// This list was generated with the iconv library using the
/// ASCII and EBCDICUS encodings.
static const Character_encodings character_encoding_array = {
  {Character_encoding{0x00u, 0x00u}, Character_encoding{0x01u, 0x01u},
   Character_encoding{0x02u, 0x02u}, Character_encoding{0x03u, 0x03u},
   Character_encoding{0x04u, 0x37u}, Character_encoding{0x05u, 0x2Du},
   Character_encoding{0x06u, 0x2Eu}, Character_encoding{0x07u, 0x2Fu},
   Character_encoding{0x08u, 0x16u}, Character_encoding{0x09u, 0x05u},
   Character_encoding{0x0Au, 0x25u}, Character_encoding{0x0Bu, 0x0Bu},
   Character_encoding{0x0Cu, 0x0Cu}, Character_encoding{0x0Du, 0x0Du},
   Character_encoding{0x0Eu, 0x0Eu}, Character_encoding{0x0Fu, 0x0Fu},
   Character_encoding{0x10u, 0x10u}, Character_encoding{0x11u, 0x11u},
   Character_encoding{0x12u, 0x12u}, Character_encoding{0x13u, 0x13u},
   Character_encoding{0x14u, 0x3Cu}, Character_encoding{0x15u, 0x3Du},
   Character_encoding{0x16u, 0x32u}, Character_encoding{0x17u, 0x26u},
   Character_encoding{0x18u, 0x18u}, Character_encoding{0x19u, 0x19u},
   Character_encoding{0x1Au, 0x3Fu}, Character_encoding{0x1Bu, 0x27u},
   Character_encoding{0x1Cu, 0x1Cu}, Character_encoding{0x1Du, 0x1Du},
   Character_encoding{0x1Eu, 0x1Eu}, Character_encoding{0x1Fu, 0x1Fu},
   Character_encoding{0x20u, 0x40u}, Character_encoding{0x21u, 0x5Au},
   Character_encoding{0x22u, 0x7Fu}, Character_encoding{0x23u, 0x7Bu},
   Character_encoding{0x24u, 0x5Bu}, Character_encoding{0x25u, 0x6Cu},
   Character_encoding{0x26u, 0x50u}, Character_encoding{0x27u, 0x7Du},
   Character_encoding{0x28u, 0x4Du}, Character_encoding{0x29u, 0x5Du},
   Character_encoding{0x2Au, 0x5Cu}, Character_encoding{0x2Bu, 0x4Eu},
   Character_encoding{0x2Cu, 0x6Bu}, Character_encoding{0x2Du, 0x60u},
   Character_encoding{0x2Eu, 0x4Bu}, Character_encoding{0x2Fu, 0x61u},
   Character_encoding{0x30u, 0xF0u}, Character_encoding{0x31u, 0xF1u},
   Character_encoding{0x32u, 0xF2u}, Character_encoding{0x33u, 0xF3u},
   Character_encoding{0x34u, 0xF4u}, Character_encoding{0x35u, 0xF5u},
   Character_encoding{0x36u, 0xF6u}, Character_encoding{0x37u, 0xF7u},
   Character_encoding{0x38u, 0xF8u}, Character_encoding{0x39u, 0xF9u},
   Character_encoding{0x3Au, 0x7Au}, Character_encoding{0x3Bu, 0x5Eu},
   Character_encoding{0x3Cu, 0x4Cu}, Character_encoding{0x3Du, 0x7Eu},
   Character_encoding{0x3Eu, 0x6Eu}, Character_encoding{0x3Fu, 0x6Fu},
   Character_encoding{0x40u, 0x7Cu}, Character_encoding{0x41u, 0xC1u},
   Character_encoding{0x42u, 0xC2u}, Character_encoding{0x43u, 0xC3u},
   Character_encoding{0x44u, 0xC4u}, Character_encoding{0x45u, 0xC5u},
   Character_encoding{0x46u, 0xC6u}, Character_encoding{0x47u, 0xC7u},
   Character_encoding{0x48u, 0xC8u}, Character_encoding{0x49u, 0xC9u},
   Character_encoding{0x4Au, 0xD1u}, Character_encoding{0x4Bu, 0xD2u},
   Character_encoding{0x4Cu, 0xD3u}, Character_encoding{0x4Du, 0xD4u},
   Character_encoding{0x4Eu, 0xD5u}, Character_encoding{0x4Fu, 0xD6u},
   Character_encoding{0x50u, 0xD7u}, Character_encoding{0x51u, 0xD8u},
   Character_encoding{0x52u, 0xD9u}, Character_encoding{0x53u, 0xE2u},
   Character_encoding{0x54u, 0xE3u}, Character_encoding{0x55u, 0xE4u},
   Character_encoding{0x56u, 0xE5u}, Character_encoding{0x57u, 0xE6u},
   Character_encoding{0x58u, 0xE7u}, Character_encoding{0x59u, 0xE8u},
   Character_encoding{0x5Au, 0xE9u}, Character_encoding{0x5Cu, 0xE0u},
   Character_encoding{0x5Fu, 0x6Du}, Character_encoding{0x60u, 0x79u},
   Character_encoding{0x61u, 0x81u}, Character_encoding{0x62u, 0x82u},
   Character_encoding{0x63u, 0x83u}, Character_encoding{0x64u, 0x84u},
   Character_encoding{0x65u, 0x85u}, Character_encoding{0x66u, 0x86u},
   Character_encoding{0x67u, 0x87u}, Character_encoding{0x68u, 0x88u},
   Character_encoding{0x69u, 0x89u}, Character_encoding{0x6Au, 0x91u},
   Character_encoding{0x6Bu, 0x92u}, Character_encoding{0x6Cu, 0x93u},
   Character_encoding{0x6Du, 0x94u}, Character_encoding{0x6Eu, 0x95u},
   Character_encoding{0x6Fu, 0x96u}, Character_encoding{0x70u, 0x97u},
   Character_encoding{0x71u, 0x98u}, Character_encoding{0x72u, 0x99u},
   Character_encoding{0x73u, 0xA2u}, Character_encoding{0x74u, 0xA3u},
   Character_encoding{0x75u, 0xA4u}, Character_encoding{0x76u, 0xA5u},
   Character_encoding{0x77u, 0xA6u}, Character_encoding{0x78u, 0xA7u},
   Character_encoding{0x79u, 0xA8u}, Character_encoding{0x7Au, 0xA9u},
   Character_encoding{0x7Bu, 0xC0u}, Character_encoding{0x7Cu, 0x4Fu},
   Character_encoding{0x7Du, 0xD0u}, Character_encoding{0x7Eu, 0xA1u},
   Character_encoding{0x7Fu, 0x07u}, Character_encoding{0xFFu, 0xFFu}}};


/// @brief Build the EBCDIC -> ASCII map
///
/// @return A list of ASCII/EBCDIC pairs sorted by EBCDIC character
///
static Character_encodings build_ebcdic_sorted_array()
{
    Character_encodings cea = character_encoding_array;

    const auto less_ebcdic = [](Character_encoding a, Character_encoding b) {
        return a.ebcdic < b.ebcdic;
    };

    std::sort(std::begin(cea), std::end(cea), less_ebcdic);

    return cea;
}

/// @brief Build the ASCII -> EBCDIC map
///
/// @return A list of ASCII/EBCDIC pairs sorted by ASCII character
///
static Character_encodings build_ascii_sorted_array()
{
    Character_encodings cea = character_encoding_array;

    const auto less_ascii = [](Character_encoding a, Character_encoding b) {
        return a.ascii < b.ascii;
    };

    std::sort(std::begin(cea), std::end(cea), less_ascii);

    return cea;
}

// We use static const variables to build the maps only once at startup.
// This could be constexpr.

/// A list of ASCII/EBCDIC pairs sorted by EBCDIC character.
static const auto ebcdic_sorted_array = build_ebcdic_sorted_array();

/// A list of ASCII/EBCDIC pairs sorted by ASCII character.
static const auto ascii_sorted_array = build_ascii_sorted_array();


}  // namespace


unsigned char to_ascii_from_ebcdic(unsigned char ebcdic_char)
{
    const auto compare_ebcdic = [](Character_encoding a, unsigned char b) {
        // Search by EBCDIC
        return a.ebcdic < b;
    };

    // Search for `ebcdic_char` in `ebcdic_sorted_array`
    const auto& ebcdic_char_it = std::lower_bound(
      std::begin(ebcdic_sorted_array), std::end(ebcdic_sorted_array),
      ebcdic_char, compare_ebcdic);

    if (ebcdic_char_it == std::end(ebcdic_sorted_array)) {
        return char_sub.ascii;
    }
    return ebcdic_char_it->ascii;
}

unsigned char to_ebcdic_from_ascii(unsigned char ascii_char)
{
    const auto compare_ascii = [](Character_encoding a, unsigned char b) {
        // Search by ASCII
        return a.ascii < b;
    };

    // Search for `ascii_char` in `ascii_sorted_array`
    const auto& ascii_char_it = std::lower_bound(
      std::begin(ascii_sorted_array), std::end(ascii_sorted_array), ascii_char,
      compare_ascii);

    if (ascii_char_it == std::end(ascii_sorted_array)) {
        return char_sub.ebcdic;
    }
    return ascii_char_it->ebcdic;
}

bool is_printable_ascii(unsigned char ascii_char)
{
    // Printable ASCII chars are in the range [0x20, 0x7E].
    return (ascii_char >= 0x20 && ascii_char <= 0x7E);
}

bool is_printable_ebcdic(unsigned char ebcdic_char)
{
    return is_printable_ascii(to_ascii_from_ebcdic(ebcdic_char));
}

}  // namespace character_encoding
}  // namespace utils
}  // namespace exseis
