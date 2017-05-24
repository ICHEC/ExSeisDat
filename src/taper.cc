/*******************************************************************************************//*!
 *   \file
 *   \author Meghan Fisher - meghan.fisher@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date April 2017
 *   \brief The taper operation
 *//*******************************************************************************************/
#include <assert.h>
#include "global.hh"
#include "share/units.hh"
#include "share/api.hh"
namespace PIOL { namespace File {

/******************************************** Core *********************************************/
/*! Apply a taper to a set of traces --> used for acutal operation during output
 * \param[in] nt The number of traces
 * \parma[in] ns The number of samples in a trace
 * \param[in] trc Vector of all traces
 * \param[in] func Weight function for the taper ramp
 * \param[in] ntpstr Length of left tail of taper
 * \param[in] ntpend Length of right tail of taper
 * \return Vector of tapered traces
 */
void taper(size_t sz, size_t ns, trace_t * trc, std::function<trace_t(trace_t weight, trace_t ramp)> func, size_t nTailLft, size_t nTailRt)
{
    assert(ns > nTailLft && ns > nTailRt);
    for (size_t i = 0; i < sz; i++)
    {
        size_t jstart;
        for (jstart = 0; jstart < ns && trc[i*ns+jstart] == 0.0f; jstart++);

        for (size_t j = jstart; j < std::min(jstart+nTailLft, ns); j++)
            trc[i*ns+j] *= func(j-jstart+1, nTailLft);

        for (size_t j = ns - nTailRt; j < ns; j++)
            trc[i*ns+j] *= func(ns - j - 1U, nTailRt);
    }
}

/******************************************** Non-Core *********************************************/
std::function<trace_t(trace_t, trace_t)> getTap(TaperType type)
{
    switch(type)
    {
        default :
        case TaperType::Linear :
        return [](trace_t weight, trace_t ramp) { return 1.0f - std::abs((weight-ramp)/ramp); };
        break;
        case TaperType::Cos :
        return [](trace_t weight, trace_t ramp) { return 0.5f + 0.5f * cos(Math::pi*(weight-ramp)/ramp); };
        break;
        case TaperType::CosSqr :
        return [](trace_t weight, trace_t ramp) { return pow(0.5f + 0.5f*cos(Math::pi*(weight-ramp)/ramp), 2.0f); };
        break;
    }
}
}}
