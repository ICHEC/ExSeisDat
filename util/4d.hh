/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \date January 2017
 *   \brief
 *   \details
 *//*******************************************************************************************/
#ifndef FOURDBIN4D_INCLUDE_GUARD
#define FOURDBIN4D_INCLUDE_GUARD
#include <iostream>
#include "cppfileapi.hh"

#define ALIGN 32U
template <class T>
using cvec = const std::vector<T>;

template <class T>
using vec = std::vector<T>;

namespace PIOL
{
void cmsg(ExSeisPIOL * piol, std::string msg);
}
#endif
