#include "set.hh"

namespace PIOL {
void Set::sort(SortType type)
{
    //TODO: Assumes the decomposition is small enough to allow this allocation
    File::Param prm(dec.second);
    src->readParam(dec.first, dec.second, &prm);
    for (size_t i = 0; i < prm.size(); i++)
        setPrm(i, Meta::tn, dec.first + i, &prm);

    auto list = Sort(piol, type, nt, dec.first, &prm);
}
}
