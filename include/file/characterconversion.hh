/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief A Conversion function to convert from EBCDIC to ASCII
 *   \details
 *//*******************************************************************************************/
#ifndef PIOLFILECHARACTERCONVERSION_INCLUDE_GUARD
#define PIOLFILECHARACTERCONVERSION_INCLUDE_GUARD
#include "global.hh"
#include <string>
namespace PIOL {
/*!\brief This function detects if the given unsigned character array contains ebcdic text. If so, it converts
 * the strong. Otherwise it is unmodified.
 * \param[in, out] piol The PIOL object
 * \param[in] file The name of the file (for error reporting)
 * \param[in] sz The length of the string.
 * \param[in, out] src The EBCDIC or ASCII string. After the function ends, the string is ASCII
 */

char ebcdicToAscii(uchar ebcdic_char);
char asciiToEbcdic(uchar ascii_char);

void getAscii(ExSeisPIOL * piol, const std::string file, csize_t sz, uchar * src);
}
#endif
