////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/Param.h"
#include "ExSeisDat/PIOL/segy_utils.hh"

namespace exseis {
namespace PIOL {

Param::Param(std::shared_ptr<Rule> r_, const size_t sz_) : r(r_), sz(sz_)
{
    f.resize(sz * r->numFloat);
    i.resize(sz * r->numLong);
    s.resize(sz * r->numShort);
    t.resize(sz * r->numIndex);

    // @todo: This must be file format agnostic
    c.resize(sz * (r->numCopy != 0 ? SEGY_utils::getMDSz() : 0));
}

Param::Param(const size_t sz_) :
    Param::Param(std::make_shared<Rule>(true, true), sz_)
{
}

Param::~Param() = default;

size_t Param::size(void) const
{
    return sz;
}

bool Param::operator==(struct Param& p) const
{
    return f == p.f && i == p.i && s == p.s && t == p.t && c == p.c;
}

size_t Param::memUsage(void) const
{
    return f.capacity() * sizeof(exseis::utils::Floating_point)
           + i.capacity() * sizeof(exseis::utils::Integer)
           + s.capacity() * sizeof(int16_t) + t.capacity() * sizeof(size_t)
           + c.capacity() * sizeof(unsigned char) + sizeof(Param)
           + r->memUsage();
}

}  // namespace PIOL
}  // namespace exseis
