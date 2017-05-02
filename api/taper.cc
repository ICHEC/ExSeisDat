/*******************************************************************************************//*!
 *   \file
 *   \author Meghan Fisher - meghan.fisher@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date April 2017
 *   \brief The taper operation
 *//*******************************************************************************************/
#include <cmath>
#include "global.hh"
#include "ops/taper.hh"
#include "fileops.hh"
#include <iostream>
namespace PIOL { namespace File {
const trace_t pi = acos(-1.0);

std::function<trace_t(trace_t, trace_t)> getTap(TaperType type)
{
    switch(type)
    {
        default :
        case TaperType::linear :
	    return [](trace_t weight, trace_t ramp) { return 1.0f - std::abs((weight-ramp)/ramp); };
        break;
        case TaperType::cos :
        return [](trace_t weight, trace_t ramp) { return 0.5f + 0.5f * cos(pi*(weight-ramp)/ramp); };
        break;
        case TaperType:: cos2 :
	    return [](trace_t weight, trace_t ramp) { return pow(0.5f + 0.5f*cos(pi*(weight-ramp)/ramp), 2.0f); };
        break;
    }
}
}}
