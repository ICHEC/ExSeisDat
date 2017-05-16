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
    if (num < 1)
        num = 1;
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
        amp += trc[j];
    size_t num = std::count_if(&trc[0], &trc[window], [](trace_t i){return i != 0.0f;});
    if (num < 1)
        num = 1;
    return normR/(std::abs(amp)/num);
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
    if (window % 2 == 0)
        window = window + 1;
    assert(ns > window);
    for (size_t i = 0; i < nt; i++)
    {
        std::vector<trace_t> trcHoldLft(&trc[i*ns], &trc[i*ns+window]);
        std::vector<trace_t> trcHoldRt(&trc[(i+1)*ns - window], &trc[(i+1)*ns]);
        std::vector<trace_t> agcHold((window/2)+1);
        for (size_t j = (window/2); j < window; j++)
        {
            std::vector<trace_t> trcWindow(&trc[i*ns+j-(window/2)], &trc[i*ns + j + (window/2) + 1]);
            agcHold[j - (window/2)] = func(window, trcWindow.data(), normR);
        }
        for (size_t j = window; j < ns - ((window/2)); j ++)
        {
            std::vector<trace_t> trcWindow(&trc[i*ns+j-(window/2)], &trc[i*ns + j + (window/2) + 1]);
            trc[i*ns+j-(window/2)-1] = agcHold[0];
            std::rotate(agcHold.begin(), agcHold.begin() + 1, agcHold.end());
            agcHold[window/2] = func(window, trcWindow.data(), normR);
        }
        for (size_t j = ns - window; j < ns - (window/2); j++)
            trc[i*ns+j] = agcHold[j-(ns-window)];
        for (size_t j = 0; j < (window/2); j++)
        {
            win = 2*j + 1;
            std::vector<trace_t> trcWindow(trcHoldLft.begin(), trcHoldLft.begin() + win);
            trc[i*ns+j] = func(win, trcWindow.data(), normR);
        }
        for (size_t j = ns - (window/2); j < ns; j++)
        {
            win = 2*(ns - j) - 1;
            std::vector<trace_t> trcWindow(trcHoldRt.end()-win, trcHoldRt.end());
            trc[i*ns+j] = func(win, trcWindow.data(), normR);
        }
    }
}
}
}
