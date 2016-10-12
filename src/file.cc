/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "file/file.hh"
namespace PIOL { namespace File {
const std::string & Interface::readText(void) const
{
    return text;
}

size_t Interface::readNs(void) const
{
    return ns;
}

geom_t Interface::readInc(void) const
{
   return inc;
}
}}
