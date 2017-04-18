/*******************************************************************************************//*!
 *   \file
 *   \author Meghan Fisher - meghan.fisher@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date April 2017
 *   \brief The Taper Operation
 *   \details The algorithm used is a nearest neighbour approach where at each iteration
 *   the lowest valued metadata entries are moved to adjacent processes with a lower rank and a
 *   sort is performed. After the sort the highest entries are moved again to the process with a
 *   higher rank. If each process has the same traces it started off with, the sort
 *   is complete.
 *//*******************************************************************************************/
#define _USE_MATH_DEFINES
#include <functional>
#include <cmath>
#include "global.hh"
#include "ops/taper.hh"
#include "fileops.hh"

namespace PIOL { namespace File {


std::function<float(float, float)> getTap(TaperType type)
{
    switch(type)
    {
        default :
        case TaperType::linear :
	return [ ](float weight, float ramp){return weight/ramp;};
        break;
        case TaperType::cos :
	return [ ](size_t weight, size_t ramp){return cos(M_PI*(1+(weight/ramp)));};
        break;
        case TaperType:: cos2 :
	  return [ ](float weight, float ramp){return pow(cos(M_PI*(ramp+weight)/ramp),2.f);};	
	break;
    };
};
}
}
