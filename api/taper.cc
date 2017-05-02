/*******************************************************************************************//*!
 *   \file
 *   \author Meghan Fisher - meghan.fisher@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date April 2017
 *   \brief The taper operation
 *//*******************************************************************************************/
#define _USE_MATH_DEFINES
#include <cmath>
#include "global.hh"
#include "fileops.hh"
#include <iostream>
namespace PIOL { namespace File {

const double pi = M_PI;

std::function<trace_t(trace_t, trace_t)> getTap(TaperType type)
{
    switch(type)
    {
        default :
        case TaperType::Linear :
	    return [](trace_t weight, trace_t ramp) { return 1.0f - std::abs((weight-ramp)/ramp); };
        break;
        case TaperType::Cos :
        return [](trace_t weight, trace_t ramp) { return 0.5f + 0.5f * cos(pi*(weight-ramp)/ramp); };
        break;
        case TaperType::CosSqr :
	    return [](trace_t weight, trace_t ramp) { return pow(0.5f + 0.5f*cos(pi*(weight-ramp)/ramp), 2.0f); };
        break;
    }
}
}}
