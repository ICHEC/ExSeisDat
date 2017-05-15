/************************************************************************ *******************//*!
 *   \file
 *   \author Meghan Fisher - meghan.fisher@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date May 2017
 *   \brief The Automatic Gain Control Operation
 *   \details The algorithm finds some type of moving average (RMS, RMS with Triangle window,
 *   Mean Abs, and Median) to trace amplitudes for visualization purposes. It can be applied to
 *   traces independantly or applies the same scalar to all traces at the same height.
*//*******************************************************************************************/
#include <vector>
#include <algorithm>
#include <cmath>
#include <assert.h>
#include "global.hh"
#include "ops/agc.hh"
#include "share/api.hh"
#include <iostream>
namespace PIOL { namespace File {

trace_t rms(size_t window, trace_t * trc, trace_t normR)
{
    trace_t amp = 0.0f;
    for (size_t j = 0; j < window; j++)
        amp += pow(trc[j], 2.0f);
    size_t num = std::count_if(&trc[0], &trc[window], [](trace_t i) {return i != 0.0f;});
    if (num < 1)
        num = 1;
    std::cout<<"Set window: "<<window<<" Set amp sum: "<<amp<<" Set num: "<<num<<std::endl;
    return normR/std::sqrt(amp/num);
}

trace_t rmsTri(size_t window, trace_t * trc, trace_t normR)
{
    trace_t amp = 0.0f;
    for (size_t j = 0; j < window; j++)
        amp += pow(trc[j] * (1.0f - abs((2.0f * j - window) / window)), 2.0f);
    size_t num = std::count_if(&trc[0], &trc[window], [](trace_t i){return i != 0.0f;});
    if (num < 1)
        num = 1;
    return normR/std::sqrt(amp/num);
}

trace_t meanAbs(size_t window, trace_t * trc, trace_t normR)
{
    trace_t amp = 0.0f;
    for (size_t j = 0; j < window; j++)
        amp += abs(trc[j]);
    size_t num = std::count_if(&trc[0], &trc[window], [](trace_t i){return i != 0.0f;});
    if (num < 1)
        num = 1;
    return normR/(amp/num);
}

trace_t median(size_t window, trace_t * trc, trace_t normR)
{
    std::sort(&trc[0], &trc[window]);
    return normR/trc[(window/2) + 1];
}

void agc(size_t nt, size_t ns, trace_t * trc, const std::function<trace_t(size_t win, trace_t * trcWin, trace_t normR)> func,
         size_t window, trace_t normR)
{
    size_t win;
    size_t nTemp = (window-2)/2;
    std::vector<trace_t> lftTemp(nTemp);
    std::vector<trace_t> rtTemp(nTemp);
    if (window % 2 == 0)
        window = window + 1;
    assert(ns > window);
    for (size_t i = 0; i < nt; i++)
    {
        for (size_t j = 0; j < nTemp; j++)
        {
            lftTemp[j]=trc[i*ns+(window/2)+1+j];
            rtTemp[j]=trc[i*ns+(ns-window+2+j)];
        }
        for (size_t j = window; j < ns; j ++)
        {
            win = window;
            std::vector<trace_t> trcWindow(&trc[i*ns+j-win], &trc[i*ns +j]);
            trc[i*ns+j-(j/2)+1] = func(win, trcWindow.data(), normR);
        }
        for (size_t j = 0; j < window - 1; j += 2)
        {
            win = j + 1;
            std::vector<trace_t> trcWindow(&trc[i*ns], &trc[i*ns +j]);
            for (size_t k = 0; k < nTemp; k++)
                trcWindow[(window/2)+1+k]= lftTemp[k];
            trc[i*ns+(j-(j/2))] = func(win, trcWindow.data(), normR);
        }
        for (size_t j = ns - ((window/2) + 1); j < ns; j += 2)
        {
            win = ns - j;
            std::vector<trace_t> trcWindow(&trc[(i+1)*ns-j], &trc[(i+1)*ns]);
            trc[i*ns+j] = func(win, trcWindow.data(), normR);
        }
    }
}
}
}
