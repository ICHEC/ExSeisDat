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

template <bool AllProc = true>
void recordTime(ExSeisPIOL * piol, std::string msg, double startTime)
{
    auto time = MPI_Wtime();

    if (AllProc)
    {
        auto pTimeTot = piol->comm->gather(vec<float>{static_cast<float>(time - startTime)});
        if (!piol->comm->getRank())
            for (size_t i = 0; i < pTimeTot.size(); i++)
                std::cout << msg << " time: " << i << " " << pTimeTot[i] << std::endl;
        piol->comm->barrier();
    }
    else
    {
        auto pTimeTot = (time - startTime);
        if (!piol->comm->getRank())
            std::cout << msg << " time: " << pTimeTot << std::endl;
    }
}
}
#endif
