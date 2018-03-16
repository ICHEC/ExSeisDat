///
/// Example: exampleFilterTaper.cc    {#examples_exampleFilterTaper_cc}
/// ==============================
///
/// @todo DOCUMENT ME - Finish documenting example.
///
/// Sort a file in 9 lines
///

#include "ExSeisDat/Flow.hh"

#include <vector>

using namespace PIOL;

int main(void)
{
    auto piol = ExSeis::New();
    Set set(piol, "Line_*", "LowpassTime");
    std::vector<float> c = {3.5, 0};
    set.temporalFilter(
      FltrType::Lowpass, FltrDmn::Time, PadType::Symmetric, 30.f, 4LU, c);
    return 0;
}
