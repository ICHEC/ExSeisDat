/*******************************************************************************************//*!
 *   \file
 *   \author Meghan Fisher - meghan.fisher@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date May 2017
 *   \brief The Automatic Gain Control Operation
 *   \details This function allows for automatic gain control to be applied to traces in order
 *   to equalize signal amplitude for display purposes. It uses a user defined window to adjust
 *   the gain using a user defined normalization value.  Gain is normalized against  a) RMS
 *   amplitude, b) RMS amplitude with triangle window, c) Mean Absolute Value amplitude or
 *   d) Median amplitude. AGC can be performed on individual traces or entire gathers, where
 *   the same gain is applied to the jth sample in every trace. The window has a spatial
 *   componet.
 *//*******************************************************************************************/
#define _USE_MATH_DEFINES
#include <functional>
#include "global.hh"
#include "ops/agc.hh"
#include "fileops.hh"

namespace PIOL { namespace File {

std::function<trace_t(size_t, trace_t * , trace_t)> agcFunc(AGCType type)
{
    switch (type)
    {
        default :
        case AGCType::RMS :
            return rms;
        break;
        case AGCType::RMSTri :
            return rmsTri;
        break;
        case AGCType::MeanAbs :
            return meanAbs;
        break;
        case AGCType::Median :
            return median;
        break;
    }
}
}
}
