#include "exseisdat/piol/WriteSEGY.hh"
#include "mockwriteinterface.hh"

namespace exseis {
namespace piol {

WriteSEGY::WriteSEGY(
  std::shared_ptr<ExSeisPIOL> piol,
  std::string name,
  const WriteSEGY::Options&) :
    m_piol(piol),
    m_name(name)
{
    mock_write_interface().ctor(this, m_piol, m_name);
}

WriteInterface::~WriteInterface()
{
    mock_write_interface().dtor(this);
}

void WriteSEGY::write_text(const std::string text)
{
    mock_write_interface().write_text(this, text);
}

void WriteSEGY::write_ns(const size_t ns)
{
    mock_write_interface().write_ns(this, ns);
}

void WriteSEGY::write_nt(const size_t nt)
{
    mock_write_interface().write_nt(this, nt);
}

void WriteSEGY::write_sample_interval(
  const exseis::utils::Floating_point sample_interval)
{
    mock_write_interface().write_sample_interval(this, sample_interval);
}

void WriteSEGY::write_trace(
  const size_t offset,
  const size_t sz,
  exseis::utils::Trace_value* trace,
  const Trace_metadata* prm,
  size_t skip)
{
    mock_write_interface().write_trace(this, offset, sz, trace, prm, skip);
}

void WriteSEGY::write_param(
  const size_t offset, const size_t sz, const Trace_metadata* prm, size_t skip)
{
    mock_write_interface().write_param(this, offset, sz, prm, skip);
}

void WriteSEGY::write_trace_non_contiguous(
  const size_t sz,
  const size_t* offset,
  exseis::utils::Trace_value* trace,
  const Trace_metadata* prm,
  size_t skip)
{
    mock_write_interface().write_trace_non_contiguous(
      this, sz, offset, trace, prm, skip);
}

void WriteSEGY::write_param_non_contiguous(
  const size_t sz, const size_t* offset, const Trace_metadata* prm, size_t skip)
{
    mock_write_interface().write_param_non_contiguous(
      this, sz, offset, prm, skip);
}

WriteSEGY::~WriteSEGY(void) {}

const std::string& WriteSEGY::file_name() const
{
    return mock_write_interface().file_name(this);
}

size_t WriteSEGY::calc_nt(void)
{
    // This is private to WriteSEGY and should never be called...
    assert(false);
    return 0;
}

}  // namespace piol
}  // namespace exseis
