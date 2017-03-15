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
const Param * PARAM_NULL = (Param *)1;

const std::string & ReadInterface::readText(void) const
{
    return text;
}

size_t ReadInterface::readNs(void) const
{
    return ns;
}

geom_t ReadInterface::readInc(void) const
{
   return inc;
}
}}
