////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Functions to convert between EBCDIC and ASCII encoding.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_ENCODING_CHARACTER_ENCODING_HH
#define EXSEISDAT_UTILS_ENCODING_CHARACTER_ENCODING_HH

#include "ExSeisDat/utils/typedefs.h"

#include <string>

namespace exseis {
namespace utils {

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

}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_ENCODING_CHARACTER_ENCODING_HH
