
/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - ruairi.short@ichec.ie - first commit
 *   \copyright LGPL v3
 *   \date February 2017
 *   \brief Sort a file in 9 lines
*//*******************************************************************************************/
#include "set.hh"
#include <vector>
using namespace PIOL;
int main(void)
{
    ExSeis piol;
    Set set(piol, "Line_*", "LowpassTime");
    std::vector<float> c = {3.5, 0};
    set.temporalFilter(FltrType::Lowpass, FltrDmn::Time, PadType::Symmetric, 30.f, 4LU, c);
    return 0;
}
