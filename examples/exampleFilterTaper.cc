///
/// Example: exampleFilterTaper.cc    {#examples_exampleFilterTaper_cc}
/// ==============================
///
/// @todo DOCUMENT ME - Finish documenting example.
///
/// Sort a file in 9 lines
///

#include "exseisdat/flow.hh"

#include <vector>

using namespace exseis::piol;
using namespace exseis::flow;

int main()
{
    auto piol = ExSeis::make();
    Set set(piol, "Line_*", "LowpassTime");
    std::vector<float> c = {3.5, 0};
    set.temporal_filter(
        FltrType::Lowpass, FltrDmn::Time, PadType::Symmetric, 30.f, 4LU, c);
    return 0;
}
