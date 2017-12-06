#include "mockwritedirect.hh"

namespace PIOL {

WriteDirect::WriteDirect(const Piol piol, const std::string name)
{
    mockWriteDirect().ctor(this, piol, name);
}

WriteDirect::WriteDirect(std::shared_ptr<WriteInterface> file)
{
    mockWriteDirect().ctor(this, file);
}

WriteDirect::~WriteDirect()
{
    mockWriteDirect().dtor(this);
}

void WriteDirect::writeText(const std::string text_)
{
    mockWriteDirect().writeText(this, text_);
}

void WriteDirect::writeNs(csize_t ns_)
{
    mockWriteDirect().writeNs(this, ns_);
}

void WriteDirect::writeNt(csize_t nt_)
{
    mockWriteDirect().writeNt(this, nt_);
}

void WriteDirect::writeInc(const geom_t inc_)
{
    mockWriteDirect().writeInc(this, inc_);
}

void WriteDirect::writeTrace(
    csize_t offset, csize_t sz, trace_t * trace, const Param * prm)
{
    mockWriteDirect().writeTrace(
        this, offset, sz, trace, prm
    );
}

void WriteDirect::writeParam(csize_t offset, csize_t sz, const Param * prm)
{
    mockWriteDirect().writeParam(this, offset, sz, prm);
}

void WriteDirect::writeTrace(
    csize_t sz, csize_t * offset, trace_t * trace, const Param * prm)
{
    mockWriteDirect().writeTrace(this, sz, offset, trace, prm);
}

void WriteDirect::writeParam(csize_t sz, csize_t * offset, const Param * prm)
{
    mockWriteDirect().writeParam(this, sz, offset, prm);
}

} // namespace PIOL
