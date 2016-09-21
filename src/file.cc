/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "file/file.hh"
#include "object/objsegy.hh"
//TODO: replace
#include "data/datampiio.hh"
#include "share/casts.hh"
#include <iostream>
namespace PIOL { namespace File {
const std::string & Interface::readText(void) const
{
    return text;
}

size_t Interface::readNs(void) const
{
    return ns;
}

size_t Interface::readNt(void) const
{
    return nt;
}

geom_t Interface::readInc(void) const
{
   return inc;
}
}}
