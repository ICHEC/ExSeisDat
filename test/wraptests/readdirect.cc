#include "mockreaddirect.hh"

namespace PIOL {
namespace File {

ReadDirect::ReadDirect(std::shared_ptr<ExSeisPIOL> piol, const std::string name)
{
    mockReadDirect().ctor(this, piol, name);
}

ReadDirect::ReadDirect(std::shared_ptr<ReadInterface> file)
{
    mockReadDirect().ctor(this, file);
}

ReadDirect::~ReadDirect()
{
    mockReadDirect().dtor(this);
}

const std::string & ReadDirect::readText() const
{
    return mockReadDirect().readText(this);
}

size_t ReadDirect::readNs() const
{
    return mockReadDirect().readNs(this);
}

size_t ReadDirect::readNt() const
{
    return mockReadDirect().readNt(this);
}

geom_t ReadDirect::readInc() const
{
    return mockReadDirect().readInc(this);
}

void ReadDirect::readTrace(
    const size_t offset, const size_t sz, trace_t * trace, Param * prm) const
{
    mockReadDirect().readTrace(this, offset, sz, trace, prm);
}

void ReadDirect::readParam(const size_t offset, const size_t sz, Param * prm) const
{
    mockReadDirect().readParam(this, offset, sz, prm);
}

void ReadDirect::readTraceNonContiguous(
    const size_t sz, const size_t * offset, trace_t * trace, Param * prm) const
{
    mockReadDirect().readTraceNonContiguous(this, sz, offset, trace, prm);
}

void ReadDirect::readTraceNonMonotonic(
    const size_t sz, const size_t * offset, trace_t * trace, Param * prm) const
{
    mockReadDirect().readTraceNonMonotonic(this, sz, offset, trace, prm);
}

void ReadDirect::readParamNonContiguous(const size_t sz, const size_t * offset, Param * prm) const
{
    mockReadDirect().readParamNonContiguous(this, sz, offset, prm);
}

} // namespace File
} // namespace PIOL
