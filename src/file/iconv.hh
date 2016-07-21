/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief A Conversion function to convert from EBCDIC to ASCII
 *   \details
 *//*******************************************************************************************/
#ifndef PIOLFILEICONV_INCLUDE_GUARD
#define PIOLFILEICONV_INCLUDE_GUARD
#include "global.hh"
#include <string>
namespace PIOL {
extern void getAscii(std::shared_ptr<ExSeisPIOL> piol, const std::string file, uchar * src, const size_t sz);
}
#endif
