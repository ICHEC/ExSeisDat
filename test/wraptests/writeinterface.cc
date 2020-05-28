#include "exseisdat/piol/file/Output_file_segy.hh"
#include "mockwriteinterface.hh"

namespace exseis {
namespace piol {
inline namespace file {

Output_file_segy::Output_file_segy(
    std::shared_ptr<ExSeisPIOL> piol,
    std::string name,
    const Output_file_segy::Options&) :
    m_piol(piol), m_name(name)
{
    mock_write_interface().ctor(this, m_piol, m_name);
}

Output_file::~Output_file()
{
    mock_write_interface().dtor(this);
}

void Output_file_segy::write_text(const std::string text)
{
    mock_write_interface().write_text(this, text);
}

void Output_file_segy::write_ns(const size_t ns)
{
    mock_write_interface().write_ns(this, ns);
}

void Output_file_segy::write_nt(const size_t nt)
{
    mock_write_interface().write_nt(this, nt);
}

void Output_file_segy::write_sample_interval(
    const exseis::utils::Floating_point sample_interval)
{
    mock_write_interface().write_sample_interval(this, sample_interval);
}

void Output_file_segy::write_trace(
    const size_t offset,
    const size_t sz,
    exseis::utils::Trace_value* trace,
    const Trace_metadata* prm,
    size_t skip)
{
    mock_write_interface().write_trace(this, offset, sz, trace, prm, skip);
}

void Output_file_segy::write_param(
    const size_t offset,
    const size_t sz,
    const Trace_metadata* prm,
    size_t skip)
{
    mock_write_interface().write_param(this, offset, sz, prm, skip);
}

void Output_file_segy::write_trace_non_contiguous(
    const size_t sz,
    const size_t* offset,
    exseis::utils::Trace_value* trace,
    const Trace_metadata* prm,
    size_t skip)
{
    mock_write_interface().write_trace_non_contiguous(
        this, sz, offset, trace, prm, skip);
}

void Output_file_segy::write_param_non_contiguous(
    const size_t sz,
    const size_t* offset,
    const Trace_metadata* prm,
    size_t skip)
{
    mock_write_interface().write_param_non_contiguous(
        this, sz, offset, prm, skip);
}

Output_file_segy::~Output_file_segy() {}

const std::string& Output_file_segy::file_name() const
{
    return mock_write_interface().file_name(this);
}

size_t Output_file_segy::calc_nt()
{
    // This is private to WriteSEGY and should never be called...
    assert(false);
    return 0;
}

}  // namespace file
}  // namespace piol
}  // namespace exseis
