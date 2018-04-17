///
/// Example: exampleSort.cc    {#examples_exampleSort_cc}
/// =======================
///
/// @todo DOCUMENT ME - Finish documenting example.
///
/// Sort a file in 9 lines
///

#include "ExSeisDat/Flow.hh"

using namespace exseis::PIOL;
using namespace exseis::Flow;

int main(void)
{
    auto piol = ExSeis::New();
    Set set(piol, "/ichec/work/exseisdat/*dat/10*/b*", "temp");
    set.sort(PIOL_SORTTYPE_OffLine);
    return 0;
}
