////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief A Conversion function to convert from EBCDIC to ASCII
/// @details
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLFILECHARACTERCONVERSION_INCLUDE_GUARD
#define PIOLFILECHARACTERCONVERSION_INCLUDE_GUARD

#include "ExSeisDat/PIOL/anc/piol.hh"
#include "ExSeisDat/PIOL/anc/global.hh"

#include <string>

namespace PIOL {

/*! Convert an EBCDIC encoded character to an ASCII encoded character. Returns
 *  the ASCII SUB character if there is no ASCII equivalent.
 *  @param[in] ebcdic_char An EBCDIC encoded character.
 *  @return An ASCII encoded character.
 */
char ebcdicToAscii(uchar ebcdic_char);

/*! Convert an ASCII encoded character to an EBCDIC encoded character. Returns
 *  the EBCDIC SUB character if there is no EBCDIC equivalent.
 *  @param[in] ascii_char An ASCII encoded character.
 *  @return An EBCDIC encoded character.
 */
char asciiToEbcdic(uchar ascii_char);

/// Determine whether an input character is a printable ASCII character
/// @param[in] ascii_char The character to test.
/// @return Whether \c ascii_char represents a printable ASCII character.
bool is_printable_ASCII(uchar ascii_char);

/// Determine whether an input character is a printable EBCDIC character
/// @param[in] ebcdic_char The character to test.
/// @return Whether \c ebcdic_char represents a printable EBCDIC character.
bool is_printable_EBCDIC(uchar ebcdic_char);

}  // namespace PIOL

#endif
