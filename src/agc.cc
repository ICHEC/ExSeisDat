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
/******************************************** Core *********************************************/
void AGC(const size_t nt, const size_t ns, trace_t * trc, const AGCFunc func, size_t window, trace_t normR)
{
    window = (window % 2LU == 0LU ? window + 1LU : window);
    assert(ns > window);
    std::vector<trace_t> trcAGC(ns+1);
    size_t win2 = window/2LU;
    for (size_t i = 0LU; i < nt; i++)
    {
        for (size_t j = 0LU;  j < win2 + 1LU; j++)
        {
            trcAGC[j] = func(&trc[i*ns], win2+j+1LU, normR, j);
            trcAGC[j + ns - win2] = func(&trc[(i+1LU)*ns - 2LU*win2 + j],  2LU*win2 - j, normR, win2);
        }

        for (size_t j = win2 + 1LU; j < ns - win2; j++)
            trcAGC[j] = func(&trc[i*ns+j - win2], window, normR, win2);

        for (size_t j = 0LU; j < ns; j++)
            trc[i*ns+j] *= trcAGC[j];
    }
}

/******************************************** Non-core *********************************************/
trace_t AGCRMS(trace_t * trc, size_t window, trace_t normR, llint)
{
    trace_t amp = trace_t(0);
    for (size_t j = 0LU; j < window; j++)
        amp += trc[j] * trc[j];
    size_t num = std::count_if(trc, &trc[window], [] (trace_t i) { return i != trace_t(0); });
    return normR / std::sqrt(amp / trace_t(!num ? 1LU : num));
}

trace_t AGCRMSTri(trace_t * trc, size_t window, trace_t normR, llint winCntr)
{
    trace_t winFullTail = std::max(std::abs(winCntr), std::abs(llint(window) - winCntr - 1L));
    trace_t amp = trace_t(0);
    for (llint j = 0LU; j < static_cast<llint>(window); j++)
        amp += pow(trc[j] * (trace_t(1) - std::abs(trace_t(j-winCntr)/winFullTail)), trace_t(2));
    size_t num = std::count_if(trc, &trc[window], [] (trace_t i) { return i != trace_t(0); });
    return normR / std::sqrt(amp / trace_t(!num ? 1LU : num));
}

trace_t AGCMeanAbs(trace_t * trc, size_t window, trace_t normR, llint)
{
    trace_t amp = trace_t(0);
    for (size_t j = 0LU; j < window; j++)
        amp += trc[j];
    size_t num = std::count_if(trc, &trc[window], [] (trace_t i) { return i != trace_t(0); });
    return normR/(std::abs(amp) / trace_t(!num ? 1LU : num));
}

//This can be optimised with std::nth_element if required.
trace_t AGCMedian(trace_t * trc, size_t window, trace_t normR, llint)
{
    std::vector<trace_t> trcTmp(trc, &trc[window]);
    std::sort(trcTmp.begin(), trcTmp.end());
    return normR/(window % 2LU == 0LU ? ((trcTmp[window/2LU]+trcTmp[window/2LU+1LU])/trace_t(2)) : trcTmp[window/2LU]);
}

AGCFunc getAGCFunc(AGCType type)
{
    switch (type)
    {
        default :
        case PIOL_AGCTYPE_RMS :
            return AGCRMS;
        case PIOL_AGCTYPE_RMSTri :
            return AGCRMSTri;
        case PIOL_AGCTYPE_MeanAbs :
            return AGCMeanAbs;
        case PIOL_AGCTYPE_Median :
            return AGCMedian;
    }
}
}}
