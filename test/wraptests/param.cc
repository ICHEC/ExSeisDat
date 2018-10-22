#include "mockparam.hh"

using namespace exseis::utils;

namespace exseis {
namespace piol {

Trace_metadata::Trace_metadata(Rule rules, size_t num_traces) :
    rules(true, true, true)
{
    mock_trace_metadata().ctor(this, rules, num_traces);
}

Trace_metadata::Trace_metadata(size_t num_traces) : rules(true, true, true)
{
    mock_trace_metadata().ctor(this, num_traces);
}

Trace_metadata::~Trace_metadata()
{
    mock_trace_metadata().dtor(this);
}

exseis::utils::Floating_point Trace_metadata::get_floating_point(
  size_t trace_index, Trace_metadata::Key entry) const
{
    return mock_trace_metadata().get_floating_point(this, trace_index, entry);
}

void Trace_metadata::set_floating_point(
  size_t trace_index,
  Trace_metadata::Key entry,
  exseis::utils::Floating_point value)
{
    mock_trace_metadata().set_floating_point(this, trace_index, entry, value);
}

exseis::utils::Integer Trace_metadata::get_integer(
  size_t trace_index, Trace_metadata::Key entry) const
{
    return mock_trace_metadata().get_integer(this, trace_index, entry);
}

void Trace_metadata::set_integer(
  size_t trace_index, Trace_metadata::Key entry, exseis::utils::Integer value)
{
    mock_trace_metadata().set_integer(this, trace_index, entry, value);
}

size_t Trace_metadata::get_index(
  size_t trace_index, Trace_metadata::Key entry) const
{
    return mock_trace_metadata().get_index(this, trace_index, entry);
}

void Trace_metadata::set_index(
  size_t trace_index, Trace_metadata::Key entry, size_t value)
{
    mock_trace_metadata().set_index(this, trace_index, entry, value);
}

void Trace_metadata::copy_entries(
  size_t trace_index,
  const Trace_metadata& source_trace_metadata,
  size_t source_trace_index)
{
    mock_trace_metadata().copy_entries(
      this, trace_index, source_trace_metadata, source_trace_index);
}

size_t Trace_metadata::size(void) const
{
    return mock_trace_metadata().size(this);
}

size_t Trace_metadata::memory_usage(void) const
{
    return mock_trace_metadata().memory_usage(this);
}

}  // namespace piol
}  // namespace exseis
