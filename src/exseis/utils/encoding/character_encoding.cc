////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief   Functions for converting between ASCII and EBCDIC encoding.
/// @details We define a list of ASCII / EBCDIC pairs of characters.  We use
///          this list to create a map from ASCII characters to the equivalent
///          EBCDIC character, and the reverse map, EBCDIC to ASCII.
///          We then provide functions for these map lookups.
////////////////////////////////////////////////////////////////////////////////

#include "exseis/utils/encoding/character_encoding.hh"

#include <algorithm>
#include <array>


namespace exseis {
inline namespace utils {
inline namespace encoding {
inline namespace character_encoding {


namespace {

/// A structure to represent a character in both ASCII and EBCDIC.
struct Character_encoding {
    /// The EBCDIC character
    unsigned char ebcdic;

    /// The ASCII character
    unsigned char ascii;
};

/// @brief The SUB character.
///
/// Used when a character is not representable in the given encoding.
constexpr Character_encoding char_sub{0x3F, 0x1A};

/// @brief A list of ASCII / EBCDIC pairs.
///
/// This list is provided as an Appendix in the SEG-Y Revision 2 paper:
///     Table 19 IBM 3270 Char Set Ref Ch 10, GA27-2837-9 April 1987
/// This paper isn't easily found on the internet, so it wasn't used here.
///
/// Some characters were missing, notably '[', ']', and '^'.
/// The output of `dd conv=ibm` seems to match the paper's dataset, and the
/// `dd` manual says it's the EBCDIC encoding for AT&T System V UNIX "IBM"
/// value.
constexpr Character_encoding character_encoding_array[] = {
    // Column 1
    /*NUL*/ {0x00, 0x00},
    /*SOH*/ {0x01, 0x01},
    /*STX*/ {0x02, 0x02},
    /*ETX*/ {0x03, 0x03},
    /*ST*/ {0x04, 0x9C},
    /*HT*/ {0x05, 0x09},
    /*SA*/ {0x06, 0x86},
    /*DEL*/ {0x07, 0x7F},
    /*EG*/ {0x08, 0x97},
    /*RI*/ {0x09, 0x8D},
    /*S2*/ {0x0A, 0x8E},
    /*VT*/ {0x0B, 0x0B},
    /*FF*/ {0x0C, 0x0C},
    /*CR*/ {0x0D, 0x0D},
    /*SO*/ {0x0E, 0x0E},
    /*SI*/ {0x0F, 0x0F},
    /*DLE*/ {0x10, 0x10},
    /*DC1*/ {0x11, 0x11},
    /*DC2*/ {0x12, 0x12},
    /*DC3*/ {0x13, 0x13},
    /*OC*/ {0x14, 0x9D},
    /*NL*/ {0x15, 0x85},
    /*BS*/ {0x16, 0x08},
    /*ES*/ {0x17, 0x87},
    /*CAN*/ {0x18, 0x18},
    /*EM*/ {0x19, 0x19},
    /*P2*/ {0x1A, 0x92},

    // Column 2
    /*S3*/ {0x1B, 0x8F},
    /*FS/IS4*/ {0x1C, 0x1C},
    /*GS/IS3*/ {0x1D, 0x1D},
    /*RS/IS2*/ {0x1E, 0x1E},
    /*US/IS1*/ {0x1F, 0x1F},
    /*PA*/ {0x20, 0x80},
    /*HO*/ {0x21, 0x81},
    /*BH*/ {0x22, 0x82},
    /*NH*/ {0x23, 0x83},
    /*IN*/ {0x24, 0x84},
    /*LF*/ {0x25, 0x0A},
    /*ETB*/ {0x26, 0x17},
    /*ESC*/ {0x27, 0x1B},
    /*HS*/ {0x28, 0x88},
    /*HJ*/ {0x29, 0x89},
    /*VS*/ {0x2A, 0x8A},
    /*PD*/ {0x2B, 0x8B},
    /*PU*/ {0x2C, 0x8C},
    /*ENQ*/ {0x2D, 0x05},
    /*ACK*/ {0x2E, 0x06},
    /*BEL*/ {0x2F, 0x07},
    /*DC*/ {0x30, 0x90},
    /*P1*/ {0x31, 0x91},
    /*SYN*/ {0x32, 0x16},
    /*TS*/ {0x33, 0x93},
    /*CC*/ {0x34, 0x94},
    /*MW*/ {0x35, 0x95},
    /*SG*/ {0x36, 0x96},
    /*EOT*/ {0x37, 0x04},
    /*SS*/ {0x38, 0x98},
    /*GC*/ {0x39, 0x99},

    // Column 3
    /*SC*/ {0x3A, 0x9A},
    /*CI*/ {0x3B, 0x9B},
    /*DC4*/ {0x3C, 0x14},
    /*NAK*/ {0x3D, 0x15},
    /*PM*/ {0x3E, 0x9E},
    /*SUB*/ {0x3F, 0x1A},
    /*SP*/ {0x40, 0x20},
    /*(cent)*/ {0x4A, 0xA2},
    /*.*/ {0x4B, 0x2E},
    /*<*/ {0x4C, 0x3C},
    /*(*/ {0x4D, 0x28},
    /*+*/ {0x4E, 0x2B},
    /*|*/ {0x4F, 0x7C},
    /*&*/ {0x50, 0x26},
    /*!*/ {0x5A, 0x21},
    /*$*/ {0x5B, 0x24},
    /*)*/ {0x5D, 0x29},
    /*;*/ {0x5E, 0x3B},
    /*(not)*/ {0x5F, 0xAC},
    /*-*/ {0x60, 0x2D},
    /* / */ {0x61, 0x2F},
    /*BB*/ {0x6A, 0xA6},
    /*,*/ {0x6B, 0x2C},
    /*%*/ {0x6C, 0x25},
    /*_*/ {0x6D, 0x5F},
    /*>*/ {0x6E, 0x3E},
    /*?*/ {0x6F, 0x3F},
    /*`*/ {0x79, 0x60},
    /*:*/ {0x7A, 0x3A},
    /*#*/ {0x7B, 0x23},
    /*@*/ {0x7C, 0x40},
    /*'*/ {0x7D, 0x27},

    // Column 4
    /*=*/{0x7E, 0x3D},
    /*"*/ {0x7F, 0x22},
    /*a*/ {0x81, 0x61},
    /*b*/ {0x82, 0x62},
    /*c*/ {0x83, 0x63},
    /*d*/ {0x84, 0x64},
    /*e*/ {0x85, 0x65},
    /*f*/ {0x86, 0x66},
    /*g*/ {0x87, 0x67},
    /*h*/ {0x88, 0x68},
    /*i*/ {0x89, 0x69},
    /*j*/ {0x91, 0x6A},
    /*k*/ {0x92, 0x6B},
    /*l*/ {0x93, 0x6C},
    /*m*/ {0x94, 0x6D},
    /*n*/ {0x95, 0x6E},
    /*o*/ {0x96, 0x6F},
    /*p*/ {0x97, 0x70},
    /*q*/ {0x98, 0x71},
    /*r*/ {0x99, 0x72},
    /*~*/ {0xA1, 0x7E},
    /*s*/ {0xA2, 0x73},
    /*t*/ {0xA3, 0x74},
    /*y*/ {0xA4, 0x75},
    /*v*/ {0xA5, 0x76},
    /*w*/ {0xA6, 0x77},
    /*x*/ {0xA7, 0x78},
    /*y*/ {0xA8, 0x79},
    /*z*/ {0xA9, 0x7A},
    /*{*/ {0xC0, 0x7B},
    /*A*/ {0xC1, 0x41},
    /*B*/ {0xC2, 0x42},
    /*C*/ {0xC3, 0x43},
    /*D*/ {0xC4, 0x44},
    /*E*/ {0xC5, 0x45},
    /*F*/ {0xC6, 0x46},
    /*G*/ {0xC7, 0x47},

    // Column 5
    /*H*/ {0xC8, 0x48},
    /*I*/ {0xC9, 0x49},
    /*}*/ {0xD0, 0x7D},
    /*J*/ {0xD1, 0x4A},
    /*K*/ {0xD2, 0x4B},
    /*L*/ {0xD3, 0x4C},
    /*M*/ {0xD4, 0x4D},
    /*N*/ {0xD5, 0x4E},
    /*O*/ {0xD6, 0x4F},
    /*P*/ {0xD7, 0x50},
    /*Q*/ {0xD8, 0x51},
    /*R*/ {0xD9, 0x52},
    /*\*/ {0xE0, 0x5C},
    /*S*/ {0xE2, 0x53},
    /*T*/ {0xE3, 0x54},
    /*U*/ {0xE4, 0x55},
    /*V*/ {0xE5, 0x56},
    /*W*/ {0xE6, 0x57},
    /*X*/ {0xE7, 0x58},
    /*Y*/ {0xE8, 0x59},
    /*Z*/ {0xE9, 0x5A},
    /*0*/ {0xF0, 0x30},
    /*1*/ {0xF1, 0x31},
    /*2*/ {0xF2, 0x32},
    /*3*/ {0xF3, 0x33},
    /*4*/ {0xF4, 0x34},
    /*5*/ {0xF5, 0x35},
    /*6*/ {0xF6, 0x36},
    /*7*/ {0xF7, 0x37},
    /*8*/ {0xF8, 0x38},
    /*9*/ {0xF9, 0x39},
    /*AC*/ {0xFF, 0x9F},

    // Column 6
    /*$*/ {0x5B, 0x24},

    // Column 7
    /***/ {0x5C, 0x2A},
    /*;*/ {0x5E, 0x3B},

    // TODO
    // [ X00 x5B Left square bracket
    // ] x00 x5E Right square bracket
    // ^ x00 x5E Circumflex, Caret

    /*[*/ {0xAD, 0x5B},
    /*]*/ {0xBD, 0x5D},
    /*^*/ {0x9A, 0x5E}

};


/// @brief A constexpr-friendly array type that can
///        map one 8-bit character set to another
class Character_map {
    constexpr static size_t m_size = 256;
    unsigned char m_data[m_size]{};

  public:
    constexpr Character_map(unsigned char sub)
    {
        for (size_t i = 0; i < m_size; i++) {
            m_data[i] = sub;
        }
    }

    template<typename Index>
    constexpr auto operator[](Index index) const
    {
        return m_data[index];
    }

    template<typename Index>
    constexpr auto& operator[](Index index)
    {
        return m_data[index];
    }
};

/// @brief Build the EBCDIC -> ASCII map
///
/// @return A list of ASCII/EBCDIC pairs sorted by EBCDIC character
///
constexpr auto build_ebcdic_to_ascii_map()
{
    Character_map ebcdic_to_ascii_map{char_sub.ascii};

    for (const auto& encoding : character_encoding_array) {
        ebcdic_to_ascii_map[encoding.ebcdic] = encoding.ascii;
    }

    return ebcdic_to_ascii_map;
}

/// @brief Build the ASCII -> EBCDIC map
///
/// @return A list of ASCII/EBCDIC pairs sorted by ASCII character
///
constexpr auto build_ascii_to_ebcdic_map()
{
    Character_map ascii_to_ebcdic_map{char_sub.ebcdic};

    for (const auto& encoding : character_encoding_array) {
        ascii_to_ebcdic_map[encoding.ascii] = encoding.ebcdic;
    }

    return ascii_to_ebcdic_map;
}

}  // namespace


unsigned char to_ascii_from_ebcdic(unsigned char ebcdic_char)
{
    // A mapping from EBCDIC values to ASCII values
    constexpr const auto ebcdic_to_ascii_map = build_ebcdic_to_ascii_map();

    return ebcdic_to_ascii_map[ebcdic_char];
}

unsigned char to_ebcdic_from_ascii(unsigned char ascii_char)
{
    // A mapping from ASCII values to EBCDIC values
    constexpr const auto ascii_to_ebcdic_map = build_ascii_to_ebcdic_map();

    return ascii_to_ebcdic_map[ascii_char];
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
}  // namespace encoding
}  // namespace utils
}  // namespace exseis
