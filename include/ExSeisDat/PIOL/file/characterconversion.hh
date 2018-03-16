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
#include "ExSeisDat/PIOL/global.hh"

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

/*! @brief This function detects if the given unsigned character array contains
 *         EBCDIC text. If so, it converts the string. Otherwise it is
 *         unmodified.
 *  @param[in,out] piol The PIOL object
 *  @param[in]     file The name of the file (for error reporting)
 *  @param[in]     sz   The length of the string.
 *  @param[in,out] src  The EBCDIC or ASCII string. After the function ends, the
 *                      string is ASCII
 */
void getAscii(ExSeisPIOL* piol, const std::string& file, size_t sz, uchar* src);

}  // namespace PIOL

#endif
