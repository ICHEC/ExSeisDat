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

trace_t rms(size_t window, trace_t * trc, trace_t normR)
{
    trace_t amp = 0.0f;
    for (size_t j = 0; j < window; j++)
        amp += pow(trc[j], 2.0f);
    size_t num = std::count_if(&trc[0], &trc[window], [](trace_t i) {return i != 0.0f;});
    return normR/std::sqrt(amp/num);
}

trace_t rmsTri(size_t window, trace_t * trc, trace_t normR)
{
    trace_t amp = 0.0f;
    for (size_t j = 0; j < window; j++)
        amp += pow(trc[j] * (1.0f - abs((2.0f * j - window) / window)), 2.0f);
    size_t num = std::count_if(&trc[0], &trc[window], [](trace_t i){return i != 0.0f;});
    return normR/std::sqrt(amp/num);
}

trace_t meanAbs(size_t window, trace_t * trc, trace_t normR)
{
    trace_t amp = 0.0f;
    for (size_t j = 0; j < window; j++)
        amp += abs(trc[j]);
    size_t num = std::count_if(&trc[0], &trc[window], [](trace_t i){return i != 0.0f;});
    return normR/(amp/num);
}

trace_t median(size_t window, trace_t * trc, trace_t normR)
{
    std::sort(&trc[0], &trc[window]);
    if (window % 2 == 1)
        return normR/trc[window/2];
    else
        return normR/((trc[window/2] + trc[(window/2) - 1]) / 2.0f);
}

void agc(size_t nt, size_t ns, trace_t * trc, const std::function<trace_t(size_t win, trace_t * trcWin, trace_t normR)> func,
         size_t window, trace_t normR)
{
    size_t win;
    assert(ns > window);
    for (size_t i = 0; i < nt; i++)
    {
        for (size_t j = 0; j < window - 1; j++)
        {
            win = j + 1;
            std::vector<trace_t> trcWindow(&trc[i*ns+j], &trc[i*ns +j + win]);
            trc[i*ns+j] = func(win, trcWindow.data(), normR);
        }
        for (size_t j = window; j < ns - window + 1; j++)
        {
            win = window;
            std::vector<trace_t> trcWindow(&trc[i*ns+j], &trc[i*ns +j + win]);
            trc[i*ns+j] = func(win, trcWindow.data(), normR);
        }
        for (size_t j = ns - window + 1; j < ns; j++)
        {
            win = ns - j;
            std::vector<trace_t> trcWindow(&trc[i*ns+j], &trc[i*ns +j + win]);
            trc[i*ns+j] = func(win, trcWindow.data(), normR);
        }
    }
}
}
}
