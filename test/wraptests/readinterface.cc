#include "exseisdat/piol/ReadSEGY.hh"
#include "mockreadinterface.hh"

namespace exseis {
namespace piol {

ReadSEGY::ReadSEGY(
  std::shared_ptr<ExSeisPIOL> piol,
  const std::string name,
  const ReadSEGY::Options& options) :
    ReadSEGY(piol, name, options, {})
{
}

ReadSEGY::ReadSEGY(
  std::shared_ptr<ExSeisPIOL> piol,
  std::string name,
  const ReadSEGY::Options& options,
  std::shared_ptr<ObjectInterface> object) :
    m_piol(piol),
    m_name(name),
    m_obj(object),
    m_sample_interval_factor(options.sample_interval_factor)
{
    mock_read_interface().ctor(this, m_piol, m_name);
}

ReadInterface::~ReadInterface()
{
    mock_read_interface().dtor(this);
}

const std::string& ReadSEGY::read_text() const
{
    return mock_read_interface().read_text(this);
}

size_t ReadSEGY::read_ns() const
{
    return mock_read_interface().read_ns(this);
}

size_t ReadSEGY::read_nt() const
{
    return mock_read_interface().read_nt(this);
}

exseis::utils::Floating_point ReadSEGY::read_sample_interval() const
{
    return mock_read_interface().read_sample_interval(this);
}

void ReadSEGY::read_trace(
  size_t offset,
  size_t sz,
  exseis::utils::Trace_value* trace,
  Trace_metadata* prm,
  size_t skip) const
{
    mock_read_interface().read_trace(this, offset, sz, trace, prm, skip);
}

void ReadSEGY::read_param(
  const size_t offset, const size_t sz, Trace_metadata* prm, size_t skip) const
{
    mock_read_interface().read_param(this, offset, sz, prm, skip);
}

void ReadSEGY::read_trace_non_contiguous(
  const size_t sz,
  const size_t* offset,
  exseis::utils::Trace_value* trace,
  Trace_metadata* prm,
  size_t skip) const
{
    mock_read_interface().read_trace_non_contiguous(
      this, sz, offset, trace, prm, skip);
}

void ReadSEGY::read_trace_non_monotonic(
  const size_t sz,
  const size_t* offset,
  exseis::utils::Trace_value* trace,
  Trace_metadata* prm,
  size_t skip) const
{
    mock_read_interface().read_trace_non_monotonic(
      this, sz, offset, trace, prm, skip);
}

void ReadSEGY::read_param_non_contiguous(
  const size_t sz, const size_t* offset, Trace_metadata* prm, size_t skip) const
{
    mock_read_interface().read_param_non_contiguous(
      this, sz, offset, prm, skip);
}

const std::string& ReadSEGY::file_name() const
{
    return mock_read_interface().file_name();
}

void ReadSEGY::read_file_headers()
{
    mock_read_interface().read_file_headers();
}


}  // namespace piol
}  // namespace exseis
