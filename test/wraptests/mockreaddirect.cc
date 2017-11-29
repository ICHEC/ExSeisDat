#include "mockreaddirect.hh"

namespace PIOL {

using namespace File;

MockReadDirect& MockReadDirect::instance()
{
    static MockReadDirect instance;
    return instance;
}

ReadDirect::ReadDirect(const Piol piol, const std::string name)
{
    MockReadDirect::instance().ctor(this, piol, name);
}

ReadDirect::ReadDirect(std::shared_ptr<ReadInterface> file)
{
    MockReadDirect::instance().ctor(this, file);
}

ReadDirect::~ReadDirect()
{
    MockReadDirect::instance().dtor(this);
}

const std::string & ReadDirect::readText() const
{
    return MockReadDirect::instance().readText(this);
}

size_t ReadDirect::readNs() const
{
    return MockReadDirect::instance().readNs(this);
}

size_t ReadDirect::readNt()
{
    return MockReadDirect::instance().readNt(this);
}

geom_t ReadDirect::readInc() const
{
    return MockReadDirect::instance().readInc(this);
}

void ReadDirect::readTrace(
    csize_t offset, csize_t sz, trace_t * trace, Param * prm) const
{
    MockReadDirect::instance().readTrace(this, offset, sz, trace, prm);
}

void ReadDirect::readParam(csize_t offset, csize_t sz, Param * prm) const
{
    MockReadDirect::instance().readParam(this, offset, sz, prm);
}

void ReadDirect::readTrace(
    csize_t sz, csize_t * offset, trace_t * trace, Param * prm) const
{
    MockReadDirect::instance().readTrace(this, sz, offset, trace, prm);
}

void ReadDirect::readTraceNonMono(
    csize_t sz, csize_t * offset, trace_t * trace, Param * prm) const
{
    MockReadDirect::instance().readTraceNonMono(this, sz, offset, trace, prm);
}

void ReadDirect::readParam(csize_t sz, csize_t * offset, Param * prm) const
{
    MockReadDirect::instance().readParam(this, sz, offset, prm);
}

} // namespace PIOL
