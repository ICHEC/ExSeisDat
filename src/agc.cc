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
namespace PIOL { namespace File {

trace_t rms(trace_t * trc, size_t strt, size_t window, trace_t normR, size_t winCntr)
{
    trace_t amp = 0.0f;
    for (size_t j = strt; j < strt + window; j++)
        amp += pow(trc[j], 2.0f);
    size_t num = std::count_if(&trc[strt], &trc[strt+window], [](trace_t i){return i != 0.0f;});
    if (num < 1)
        num = 1;
    return normR/std::sqrt(amp/num);
}

trace_t rmsTri(trace_t * trc, size_t strt, size_t window, trace_t normR, size_t winCntr)
{
    trace_t amp = 0.0f;
    for (size_t j = strt; j < strt + window; j++)
        amp += pow(trc[j] * (1.0f - abs(((2.0f * j)+4.0f*(winCntr-window-strt+1))/(window-winCntr+strt-1))), 2.0f);
    size_t num = std::count_if(&trc[strt], &trc[strt+window], [](trace_t i){return i != 0.0f;});
    if (num < 1)
        num = 1;
    return normR/std::sqrt(amp/num);
}

trace_t meanAbs(trace_t * trc, size_t strt, size_t window, trace_t normR, size_t winCntr)
{
    trace_t amp = 0.0f;
    for (size_t j = strt; j < strt + window; j++)
        amp += trc[j];
    size_t num = std::count_if(&trc[strt], &trc[strt+window], [](trace_t i){return i != 0.0f;});
    if (num < 1)
        num = 1;
    return normR/(std::abs(amp)/num);
}

trace_t median(trace_t * trc, size_t strt, size_t window, trace_t normR, size_t winCntr)
{
    std::sort(&trc[strt], &trc[strt+window]);
    return normR/trc[winCntr];
}

void agc(size_t nt, size_t ns, trace_t * trc, const std::function<trace_t(trace_t * trcWin, size_t strt,
         size_t win, trace_t normR, size_t winCntr)> func, size_t window, trace_t normR)
{
    if (window % 2 == 0)
        window = window + 1;
    assert(ns > window);
    for (size_t i = 0; i < nt; i++)
    {
        std::vector<trace_t> trcAGC(ns);
        for (size_t j = 0;  j < window/2U + 1; j++)
            trcAGC[j]=func(trc, i*ns, (window/2U)+j+1, normR, j);
        for (size_t j = window/2U + 1 ; j < ns - window/2U; j++)
            trcAGC[j]=func(trc, i*ns+j - window/2U, window, normR, j);
        for (size_t j = ns - window/2U; j < ns; j++)
            trcAGC[j]= func(trc, i*ns + j - (window/2U), ns - j + (window/2U), normR, j+(window/2U));
        for (size_t j = 0; j < ns; j++)
            trc[i*ns+j] *= trcAGC[j];
    }
}
}
}
