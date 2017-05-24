/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date May 2017
 *   \brief
*//*******************************************************************************************/
//TODO: Note, these functions are intentionally specific because we need
//      further use cases to generalise the functionality.

#ifndef PIOLOPSGATHER_INCLUDE_GUARD
#define PIOLOPSGATHER_INCLUDE_GUARD
#include "share/uniray.hh"
#include "file/file.hh"
namespace PIOL { namespace File {
Uniray<size_t, llint, llint> getGathers(ExSeisPIOL * piol, File::Param * prm);
Uniray<size_t, llint, llint> getIlXlGathers(ExSeisPIOL * piol, File::ReadInterface * file);
}}
#endif
