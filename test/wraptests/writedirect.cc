#include "mockwritedirect.hh"

namespace exseis {
namespace PIOL {

WriteDirect::WriteDirect(
  std::shared_ptr<ExSeisPIOL> piol, const std::string name)
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

void WriteDirect::writeNs(const size_t ns_)
{
    mockWriteDirect().writeNs(this, ns_);
}

void WriteDirect::writeNt(const size_t nt_)
{
    mockWriteDirect().writeNt(this, nt_);
}

void WriteDirect::writeInc(const exseis::utils::Floating_point inc_)
{
    mockWriteDirect().writeInc(this, inc_);
}

void WriteDirect::writeTrace(
  const size_t offset,
  const size_t sz,
  exseis::utils::Trace_value* trace,
  const Param* prm)
{
    mockWriteDirect().writeTrace(this, offset, sz, trace, prm);
}

void WriteDirect::writeParam(
  const size_t offset, const size_t sz, const Param* prm)
{
    mockWriteDirect().writeParam(this, offset, sz, prm);
}

void WriteDirect::writeTraceNonContiguous(
  const size_t sz,
  const size_t* offset,
  exseis::utils::Trace_value* trace,
  const Param* prm)
{
    mockWriteDirect().writeTraceNonContiguous(this, sz, offset, trace, prm);
}

void WriteDirect::writeParamNonContiguous(
  const size_t sz, const size_t* offset, const Param* prm)
{
    mockWriteDirect().writeParamNonContiguous(this, sz, offset, prm);
}

}  // namespace PIOL
}  // namespace exseis
