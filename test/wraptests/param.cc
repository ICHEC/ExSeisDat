#include "mockparam.hh"
#include <iostream>

namespace PIOL {
namespace File {

Param::Param(std::shared_ptr<Rule> r_, csize_t sz)
{
    mockParam().ctor(this, r_, sz);
}

Param::Param(csize_t sz)
{
    mockParam().ctor(this, sz);
}

size_t Param::size(void) const
{
    return mockParam().size(this);
}

size_t Param::memUsage(void) const
{
    return mockParam().memUsage(this);
}

void cpyPrm(csize_t j, const Param * src, csize_t k, Param * dst)
{
    mockParamFreeFunctions().cpyPrm(j, src, k, dst);
}


template <>
int16_t getPrm<int16_t>(size_t i, Meta entry, const Param * prm) {
    return mockParamFreeFunctions().getPrm_int16_t(i, entry, prm);
}

template <>
PIOL::llint getPrm<PIOL::llint>(size_t i, Meta entry, const Param * prm) {
    return mockParamFreeFunctions().getPrm_llint(i, entry, prm);
}

template <>
geom_t getPrm<geom_t>(size_t i, Meta entry, const Param * prm) {
    return mockParamFreeFunctions().getPrm_geom_t(i, entry, prm);
}

template<>
void setPrm<int16_t>(csize_t i, const Meta entry, int16_t ret, Param * prm) {
    mockParamFreeFunctions().setPrm_int16_t(i, entry, ret, prm);
}

template<>
void setPrm<llint>(csize_t i, const Meta entry, llint ret, Param * prm) {
    mockParamFreeFunctions().setPrm_llint(i, entry, ret, prm);
}

template<>
void setPrm<geom_t>(csize_t i, const Meta entry, geom_t ret, Param * prm) {
    mockParamFreeFunctions().setPrm_geom_t(i, entry, ret, prm);
}

} // namespace File
} // namespace PIOL
