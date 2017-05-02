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
#include <iostream>
namespace PIOL { namespace File {


std::function<float(float, float)> getTap(TaperType type)
{
    switch(type)
    {
        default :
        case TaperType::Linear :
	  return [ ](float weight, float ramp){return 1.-std::abs((weight-ramp)/ramp);};
        break;
        case TaperType::Cos :
	  return [ ](float weight, float ramp){return .5 + (.5*cos(M_PI*(weight-ramp)/ramp));};
        break;
        case TaperType:: Cos2 :
	return [ ](float weight, float ramp){return pow(.5 +(.5*cos(M_PI*(weight-ramp)/ramp)),2.f);};	
	break;
    };
}
  };
}
