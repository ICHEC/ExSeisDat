///
/// Example: sort.cc    {#examples_sort_cc}
/// ================
///
/// @todo DOCUMENT ME - Finish documenting example.
///
/// Sort a file in 9 lines
///

#include "exseisdat/flow.hh"

using namespace exseis::piol;
using namespace exseis::flow;

int main()
{
    auto piol = ExSeis::make();
    Set set(piol, "/ichec/work/exseisdat/*dat/10*/b*", "temp");
    set.sort(SortType::OffLine);
    return 0;
}
