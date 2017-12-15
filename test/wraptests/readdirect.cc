#include "mockreaddirect.hh"

namespace PIOL {

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

size_t ReadDirect::readNt()
{
    return mockReadDirect().readNt(this);
}

geom_t ReadDirect::readInc() const
{
    return mockReadDirect().readInc(this);
}

void ReadDirect::readTrace(
    csize_t offset, csize_t sz, trace_t * trace, Param * prm) const
{
    mockReadDirect().readTrace(this, offset, sz, trace, prm);
}

void ReadDirect::readParam(csize_t offset, csize_t sz, Param * prm) const
{
    mockReadDirect().readParam(this, offset, sz, prm);
}

void ReadDirect::readTrace(
    csize_t sz, csize_t * offset, trace_t * trace, Param * prm) const
{
    mockReadDirect().readTrace(this, sz, offset, trace, prm);
}

void ReadDirect::readTraceNonMono(
    csize_t sz, csize_t * offset, trace_t * trace, Param * prm) const
{
    mockReadDirect().readTraceNonMono(this, sz, offset, trace, prm);
}

void ReadDirect::readParam(csize_t sz, csize_t * offset, Param * prm) const
{
    mockReadDirect().readParam(this, sz, offset, prm);
}

} // namespace PIOL
