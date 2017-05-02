/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date November 2016
 *   \brief 
 *//*******************************************************************************************/
#ifndef PIOLOPSTAPER_INCLUDE_GUARD
#define PIOLOPSTAPER_INCLUDE_GUARD
#include <algorithm>
#include <iterator>
#include <functional>
#include "global.hh"
#include "share/api.hh"

namespace PIOL { namespace File {

/*! Get tapering weight functions for built in taper types
 * \param[in] type Type of taper function
 * \return Lambda function of taper weights
 */
std::function<float(float,float)> getTap(TaperType type);

}
}
#endif
