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
/*! Find the inline/crossline for each il/xl gather and the number of traces per gather using
 *  the parameters from the file provided.
 * \param[in] piol The piol object.
 * \param[in] file The file which has il/xl gathers.
 * \return Return an 'array' of tuples. Each tuple corresponds to each gather. Tuple elements:
 *         1) Number of traces in the gather, 2) inline, 3) crossline.
 */
Uniray<size_t, llint, llint> getIlXlGathers(ExSeisPIOL * piol, File::ReadInterface * file);
}}
#endif
