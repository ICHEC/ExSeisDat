#include "set.hh"
using namespace PIOL;
int main(void)
{
    ExSeis piol;
    Set set(piol, "/ichec/work/exseisdat/dat/10*/b*", "temp");
    set.sort(SortType::OffLine);
    return 0;
}
