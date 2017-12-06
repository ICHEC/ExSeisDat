#include "mockparam.hh"

namespace PIOL {

Param::Param(std::shared_ptr<Rule> r_, csize_t sz)
{
    //mockParam().ctor(this, r_, sz);
}

Param::Param(csize_t sz)
{
    //mockParam().ctor(this, sz);
}

size_t Param::size(void) const
{
    return 0; //mockParam().size(this);
}

size_t Param::memUsage(void) const
{
    return 0; //mockParam().memUsage(this);
}

void File::cpyPrm(csize_t j, const Param * src, csize_t k, Param * dst)
{
    return ;//mockParamFreeFunctions().cpyPrm(j, src, k, dst);
}

}
