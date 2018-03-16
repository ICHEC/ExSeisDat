////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief Functions for converting between ASCII and EBCDIC encoding.
/// @details We define a list of ASCII / EBCDIC pairs of characters.  We use
///          this list to create a map from ASCII characters to the equivalent
///          EBCDIC character, and the reverse map, EBCDIC to ASCII.
///          We then provide functions for these map lookups.
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/file/characterconversion.hh"

#include <algorithm>
#include <array>

namespace PIOL {


/// A structure to represent an ASCII / EBCDIC equivalent pair
struct EbcdicAsciiPair {
    /// Construct an ASCII / EBCDIC equivalent pair
    /// @param ascii  The ASCII char code
    /// @param ebcdic The EBCDIC char code
    explicit EbcdicAsciiPair(uchar ascii, uchar ebcdic) :
        ascii(ascii),
        ebcdic(ebcdic)
    {
    }

    /// The ASCII character
    uchar ascii;

    /// The EBCDIC character
    uchar ebcdic;
};

/// The SUB character.
/// Used when a character is not representable in the given encoding.
static const EbcdicAsciiPair char_sub{0x1Au, 0x3Fu};

/// The array type for all the ASCII/EBCDIC pairs.
using EbcdicAsciiPairs = std::array<EbcdicAsciiPair, 126>;

/// A list of ASCII / EBCDIC pairs.
/// This list was generated with the iconv library using the
/// ASCII and EBCDICUS encodings.
static const EbcdicAsciiPairs ebcdicAsciiPairs = {
  {EbcdicAsciiPair{0x00u, 0x00u}, EbcdicAsciiPair{0x01u, 0x01u},
   EbcdicAsciiPair{0x02u, 0x02u}, EbcdicAsciiPair{0x03u, 0x03u},
   EbcdicAsciiPair{0x04u, 0x37u}, EbcdicAsciiPair{0x05u, 0x2Du},
   EbcdicAsciiPair{0x06u, 0x2Eu}, EbcdicAsciiPair{0x07u, 0x2Fu},
   EbcdicAsciiPair{0x08u, 0x16u}, EbcdicAsciiPair{0x09u, 0x05u},
   EbcdicAsciiPair{0x0Au, 0x25u}, EbcdicAsciiPair{0x0Bu, 0x0Bu},
   EbcdicAsciiPair{0x0Cu, 0x0Cu}, EbcdicAsciiPair{0x0Du, 0x0Du},
   EbcdicAsciiPair{0x0Eu, 0x0Eu}, EbcdicAsciiPair{0x0Fu, 0x0Fu},
   EbcdicAsciiPair{0x10u, 0x10u}, EbcdicAsciiPair{0x11u, 0x11u},
   EbcdicAsciiPair{0x12u, 0x12u}, EbcdicAsciiPair{0x13u, 0x13u},
   EbcdicAsciiPair{0x14u, 0x3Cu}, EbcdicAsciiPair{0x15u, 0x3Du},
   EbcdicAsciiPair{0x16u, 0x32u}, EbcdicAsciiPair{0x17u, 0x26u},
   EbcdicAsciiPair{0x18u, 0x18u}, EbcdicAsciiPair{0x19u, 0x19u},
   EbcdicAsciiPair{0x1Au, 0x3Fu}, EbcdicAsciiPair{0x1Bu, 0x27u},
   EbcdicAsciiPair{0x1Cu, 0x1Cu}, EbcdicAsciiPair{0x1Du, 0x1Du},
   EbcdicAsciiPair{0x1Eu, 0x1Eu}, EbcdicAsciiPair{0x1Fu, 0x1Fu},
   EbcdicAsciiPair{0x20u, 0x40u}, EbcdicAsciiPair{0x21u, 0x5Au},
   EbcdicAsciiPair{0x22u, 0x7Fu}, EbcdicAsciiPair{0x23u, 0x7Bu},
   EbcdicAsciiPair{0x24u, 0x5Bu}, EbcdicAsciiPair{0x25u, 0x6Cu},
   EbcdicAsciiPair{0x26u, 0x50u}, EbcdicAsciiPair{0x27u, 0x7Du},
   EbcdicAsciiPair{0x28u, 0x4Du}, EbcdicAsciiPair{0x29u, 0x5Du},
   EbcdicAsciiPair{0x2Au, 0x5Cu}, EbcdicAsciiPair{0x2Bu, 0x4Eu},
   EbcdicAsciiPair{0x2Cu, 0x6Bu}, EbcdicAsciiPair{0x2Du, 0x60u},
   EbcdicAsciiPair{0x2Eu, 0x4Bu}, EbcdicAsciiPair{0x2Fu, 0x61u},
   EbcdicAsciiPair{0x30u, 0xF0u}, EbcdicAsciiPair{0x31u, 0xF1u},
   EbcdicAsciiPair{0x32u, 0xF2u}, EbcdicAsciiPair{0x33u, 0xF3u},
   EbcdicAsciiPair{0x34u, 0xF4u}, EbcdicAsciiPair{0x35u, 0xF5u},
   EbcdicAsciiPair{0x36u, 0xF6u}, EbcdicAsciiPair{0x37u, 0xF7u},
   EbcdicAsciiPair{0x38u, 0xF8u}, EbcdicAsciiPair{0x39u, 0xF9u},
   EbcdicAsciiPair{0x3Au, 0x7Au}, EbcdicAsciiPair{0x3Bu, 0x5Eu},
   EbcdicAsciiPair{0x3Cu, 0x4Cu}, EbcdicAsciiPair{0x3Du, 0x7Eu},
   EbcdicAsciiPair{0x3Eu, 0x6Eu}, EbcdicAsciiPair{0x3Fu, 0x6Fu},
   EbcdicAsciiPair{0x40u, 0x7Cu}, EbcdicAsciiPair{0x41u, 0xC1u},
   EbcdicAsciiPair{0x42u, 0xC2u}, EbcdicAsciiPair{0x43u, 0xC3u},
   EbcdicAsciiPair{0x44u, 0xC4u}, EbcdicAsciiPair{0x45u, 0xC5u},
   EbcdicAsciiPair{0x46u, 0xC6u}, EbcdicAsciiPair{0x47u, 0xC7u},
   EbcdicAsciiPair{0x48u, 0xC8u}, EbcdicAsciiPair{0x49u, 0xC9u},
   EbcdicAsciiPair{0x4Au, 0xD1u}, EbcdicAsciiPair{0x4Bu, 0xD2u},
   EbcdicAsciiPair{0x4Cu, 0xD3u}, EbcdicAsciiPair{0x4Du, 0xD4u},
   EbcdicAsciiPair{0x4Eu, 0xD5u}, EbcdicAsciiPair{0x4Fu, 0xD6u},
   EbcdicAsciiPair{0x50u, 0xD7u}, EbcdicAsciiPair{0x51u, 0xD8u},
   EbcdicAsciiPair{0x52u, 0xD9u}, EbcdicAsciiPair{0x53u, 0xE2u},
   EbcdicAsciiPair{0x54u, 0xE3u}, EbcdicAsciiPair{0x55u, 0xE4u},
   EbcdicAsciiPair{0x56u, 0xE5u}, EbcdicAsciiPair{0x57u, 0xE6u},
   EbcdicAsciiPair{0x58u, 0xE7u}, EbcdicAsciiPair{0x59u, 0xE8u},
   EbcdicAsciiPair{0x5Au, 0xE9u}, EbcdicAsciiPair{0x5Cu, 0xE0u},
   EbcdicAsciiPair{0x5Fu, 0x6Du}, EbcdicAsciiPair{0x60u, 0x79u},
   EbcdicAsciiPair{0x61u, 0x81u}, EbcdicAsciiPair{0x62u, 0x82u},
   EbcdicAsciiPair{0x63u, 0x83u}, EbcdicAsciiPair{0x64u, 0x84u},
   EbcdicAsciiPair{0x65u, 0x85u}, EbcdicAsciiPair{0x66u, 0x86u},
   EbcdicAsciiPair{0x67u, 0x87u}, EbcdicAsciiPair{0x68u, 0x88u},
   EbcdicAsciiPair{0x69u, 0x89u}, EbcdicAsciiPair{0x6Au, 0x91u},
   EbcdicAsciiPair{0x6Bu, 0x92u}, EbcdicAsciiPair{0x6Cu, 0x93u},
   EbcdicAsciiPair{0x6Du, 0x94u}, EbcdicAsciiPair{0x6Eu, 0x95u},
   EbcdicAsciiPair{0x6Fu, 0x96u}, EbcdicAsciiPair{0x70u, 0x97u},
   EbcdicAsciiPair{0x71u, 0x98u}, EbcdicAsciiPair{0x72u, 0x99u},
   EbcdicAsciiPair{0x73u, 0xA2u}, EbcdicAsciiPair{0x74u, 0xA3u},
   EbcdicAsciiPair{0x75u, 0xA4u}, EbcdicAsciiPair{0x76u, 0xA5u},
   EbcdicAsciiPair{0x77u, 0xA6u}, EbcdicAsciiPair{0x78u, 0xA7u},
   EbcdicAsciiPair{0x79u, 0xA8u}, EbcdicAsciiPair{0x7Au, 0xA9u},
   EbcdicAsciiPair{0x7Bu, 0xC0u}, EbcdicAsciiPair{0x7Cu, 0x4Fu},
   EbcdicAsciiPair{0x7Du, 0xD0u}, EbcdicAsciiPair{0x7Eu, 0xA1u},
   EbcdicAsciiPair{0x7Fu, 0x07u}, EbcdicAsciiPair{0xFFu, 0xFFu}}};


//
// We define static functions for building the maps which we will use to
// statically initialize the EBCDIC -> ASCII and ASCII -> EBCDIC maps.
// The maps are defined as lists sorted by the key value (i.e. by ASCII value
// or by EBCDIC value), and lookup is done using a sorted list lookup.
//


/// Build the EBCDIC -> ASCII map
/// @return A list of ASCII/EBCDIC pairs sorted by EBCDIC character
static EbcdicAsciiPairs buildEbcdicToAsciiMap()
{
    EbcdicAsciiPairs a = ebcdicAsciiPairs;
    std::sort(
      std::begin(a), std::end(a),
      [](EbcdicAsciiPair a, EbcdicAsciiPair b) { return a.ebcdic < b.ebcdic; });
    return a;
}

/// Build the ASCII -> EBCDIC map
/// @return A list of ASCII/EBCDIC pairs sorted by ASCII character
static EbcdicAsciiPairs buildAsciiToEbcdicMap()
{
    EbcdicAsciiPairs a = ebcdicAsciiPairs;
    std::sort(
      std::begin(a), std::end(a),
      [](EbcdicAsciiPair a, EbcdicAsciiPair b) { return a.ascii < b.ascii; });
    return a;
}

// We use static const (namespace level) variables to build the maps only once
// at startup.

/// A list of ASCII/EBCDIC pairs sorted by EBCDIC character.
static const auto ebcdicToAsciiMap = buildEbcdicToAsciiMap();

/// A list of ASCII/EBCDIC pairs sorted by ASCII character.
static const auto asciiToEbcdicMap = buildAsciiToEbcdicMap();

// The accessor functions

char ebcdicToAscii(uchar ebcdic_char)
{
    // Use std::lower_bound for a binary search lookup
    const auto& ascii_char_it = std::lower_bound(
      std::begin(ebcdicToAsciiMap), std::end(ebcdicToAsciiMap), ebcdic_char,
      [](EbcdicAsciiPair a, uchar b) {
          // Search by EBCDIC
          return a.ebcdic < b;
      });
    if (ascii_char_it == std::end(ebcdicToAsciiMap)) {
        return char_sub.ascii;
    }
    return ascii_char_it->ascii;
}

char asciiToEbcdic(uchar ascii_char)
{
    const auto& ebcdic_char_it = std::lower_bound(
      std::begin(ebcdicToAsciiMap), std::end(ebcdicToAsciiMap), ascii_char,
      [](EbcdicAsciiPair a, uchar b) {
          // Search by ASCII
          return a.ascii < b;
      });
    if (ebcdic_char_it == std::end(ebcdicToAsciiMap)) {
        return char_sub.ebcdic;
    }
    return ebcdic_char_it->ebcdic;
}

void getAscii(ExSeisPIOL*, const std::string&, size_t sz, uchar* src)
{
    for (size_t i = 0; i < sz; i++) {
        src[i] = ebcdicToAscii(src[i]);
    }
}

}  // namespace PIOL
