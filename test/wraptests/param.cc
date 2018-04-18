#include "mockparam.hh"

using namespace exseis::utils;

namespace exseis {
namespace PIOL {

Param::Param(std::shared_ptr<Rule> r_, const size_t sz)
{
    mockParam().ctor(this, r_, sz);
}

Param::Param(const size_t sz)
{
    mockParam().ctor(this, sz);
}

Param::~Param()
{
    mockParam().dtor(this);
}

size_t Param::size(void) const
{
    return mockParam().size(this);
}

size_t Param::memUsage(void) const
{
    return mockParam().memUsage(this);
}

void param_utils::cpyPrm(
  const size_t j, const Param* src, const size_t k, Param* dst)
{
    mockParamFreeFunctions().cpyPrm(j, src, k, dst);
}


template<>
int16_t param_utils::getPrm<int16_t>(size_t i, Meta entry, const Param* prm)
{
    return mockParamFreeFunctions().getPrm_int16_t(i, entry, prm);
}

template<>
exseis::utils::Integer param_utils::getPrm<exseis::utils::Integer>(
  size_t i, Meta entry, const Param* prm)
{
    return mockParamFreeFunctions().getPrm_Integer(i, entry, prm);
}

template<>
exseis::utils::Floating_point
param_utils::getPrm<exseis::utils::Floating_point>(
  size_t i, Meta entry, const Param* prm)
{
    return mockParamFreeFunctions().getPrm_Floating_point(i, entry, prm);
}

template<>
void param_utils::setPrm<int16_t>(
  const size_t i, const Meta entry, int16_t ret, Param* prm)
{
    mockParamFreeFunctions().setPrm_int16_t(i, entry, ret, prm);
}

template<>
void param_utils::setPrm<exseis::utils::Integer>(
  const size_t i, const Meta entry, exseis::utils::Integer ret, Param* prm)
{
    mockParamFreeFunctions().setPrm_Integer(i, entry, ret, prm);
}

template<>
void param_utils::setPrm<exseis::utils::Floating_point>(
  const size_t i,
  const Meta entry,
  exseis::utils::Floating_point ret,
  Param* prm)
{
    mockParamFreeFunctions().setPrm_Floating_point(i, entry, ret, prm);
}

}  // namespace PIOL
}  // namespace exseis
