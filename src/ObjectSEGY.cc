////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/Binary_file.hh"
#include "exseisdat/piol/ExSeisPIOL.hh"
#include "exseisdat/piol/ObjectSEGY.hh"
#include "exseisdat/piol/mpi/MPI_Binary_file.hh"
#include "exseisdat/piol/segy/utils.hh"

namespace exseis {
namespace piol {

//////////////////////      Constructor & Destructor      //////////////////////
ObjectSEGY::ObjectSEGY(
  std::shared_ptr<ExSeisPIOL> piol,
  std::string name,
  std::shared_ptr<Binary_file> data) :
    m_piol(piol),
    m_name(name),
    m_data(data)
{
}

std::shared_ptr<ExSeisPIOL> ObjectSEGY::piol() const
{
    return m_piol;
}

std::string ObjectSEGY::name() const
{
    return m_name;
}

std::shared_ptr<Binary_file> ObjectSEGY::data() const
{
    return m_data;
}

size_t ObjectSEGY::get_file_size(void) const
{
    return m_data->get_file_size();
}

void ObjectSEGY::set_file_size(const size_t sz) const
{
    return m_data->set_file_size(sz);
}

void ObjectSEGY::read_ho(unsigned char* ho) const
{
    m_data->read(0LU, segy::segy_binary_file_header_size(), ho);
}

void ObjectSEGY::should_write_file_header(const unsigned char* ho) const
{
    if (ho != nullptr) {
        m_data->write(0LU, segy::segy_binary_file_header_size(), ho);
    }
    else {
        m_data->write(0LU, 0U, ho);
    }
}

void ObjectSEGY::read_trace(
  const size_t offset, const size_t ns, const size_t sz, unsigned char* d) const
{
    m_data->read(
      segy::segy_trace_location(offset, ns), sz * segy::segy_trace_size(ns), d);
}

void ObjectSEGY::write_trace(
  const size_t offset,
  const size_t ns,
  const size_t sz,
  const unsigned char* d) const
{
    m_data->write(
      segy::segy_trace_location(offset, ns), sz * segy::segy_trace_size(ns), d);
}

void ObjectSEGY::read_trace_metadata(
  const size_t offset,
  const size_t ns,
  const size_t sz,
  unsigned char* md) const
{
    m_data->read_noncontiguous(
      segy::segy_trace_location(offset, ns), segy::segy_trace_header_size(),
      segy::segy_trace_size(ns), sz, md);
}

void ObjectSEGY::write_trace_metadata(
  const size_t offset,
  const size_t ns,
  const size_t sz,
  const unsigned char* md) const
{
    m_data->write_noncontiguous(
      segy::segy_trace_location(offset, ns), segy::segy_trace_header_size(),
      segy::segy_trace_size(ns), sz, md);
}

void ObjectSEGY::read_trace_data(
  const size_t offset,
  const size_t ns,
  const size_t sz,
  unsigned char* df) const
{
    m_data->read_noncontiguous(
      segy::segy_trace_data_location(offset, ns),
      segy::segy_trace_data_size(ns), segy::segy_trace_size(ns), sz, df);
}

void ObjectSEGY::write_trace_data(
  const size_t offset,
  const size_t ns,
  const size_t sz,
  const unsigned char* df) const
{
    m_data->write_noncontiguous(
      segy::segy_trace_data_location(offset, ns),
      segy::segy_trace_data_size(ns), segy::segy_trace_size(ns), sz, df);
}

// TODO: Add optional validation in this layer?
void ObjectSEGY::read_trace(
  const size_t* offset,
  const size_t ns,
  const size_t sz,
  unsigned char* d) const
{
    std::vector<size_t> dooff(sz);

    for (size_t i = 0; i < sz; i++) {
        dooff[i] = segy::segy_trace_location(offset[i], ns);
    }

    m_data->read_noncontiguous_irregular(
      segy::segy_trace_size(ns), sz, dooff.data(), d);
}

void ObjectSEGY::write_trace(
  const size_t* offset,
  const size_t ns,
  const size_t number_of_traces,
  const unsigned char* buffer) const
{
    std::vector<size_t> dooff(number_of_traces);

    for (size_t i = 0; i < number_of_traces; i++) {
        dooff[i] = segy::segy_trace_location(offset[i], ns);
    }

    m_data->write_noncontiguous_irregular(
      segy::segy_trace_size(ns), number_of_traces, dooff.data(), buffer);
}

void ObjectSEGY::read_trace_metadata(
  const size_t* offset,
  const size_t ns,
  const size_t sz,
  unsigned char* md) const
{
    std::vector<size_t> dooff(sz);

    for (size_t i = 0; i < sz; i++) {
        dooff[i] = segy::segy_trace_location(offset[i], ns);
    }

    m_data->read_noncontiguous_irregular(
      segy::segy_trace_header_size(), sz, dooff.data(), md);
}

void ObjectSEGY::write_trace_metadata(
  const size_t* offset,
  const size_t ns,
  const size_t sz,
  const unsigned char* md) const
{
    std::vector<size_t> dooff(sz);

    for (size_t i = 0; i < sz; i++) {
        dooff[i] = segy::segy_trace_location(offset[i], ns);
    }

    m_data->write_noncontiguous_irregular(
      segy::segy_trace_header_size(), sz, dooff.data(), md);
}

void ObjectSEGY::read_trace_data(
  const size_t* offset,
  const size_t ns,
  const size_t sz,
  unsigned char* df) const
{
    if (ns == 0) {
        return;
    }

    std::vector<size_t> dooff(sz);

    for (size_t i = 0; i < sz; i++) {
        dooff[i] = segy::segy_trace_data_location(offset[i], ns);
    }

    m_data->read_noncontiguous_irregular(
      segy::segy_trace_data_size(ns), sz, dooff.data(), df);
}

void ObjectSEGY::write_trace_data(
  const size_t* offset,
  const size_t ns,
  const size_t sz,
  const unsigned char* df) const
{
    if (ns == 0) {
        return;
    }

    std::vector<size_t> dooff(sz);

    for (size_t i = 0; i < sz; i++) {
        dooff[i] = segy::segy_trace_data_location(offset[i], ns);
    }

    m_data->write_noncontiguous_irregular(
      segy::segy_trace_data_size(ns), sz, dooff.data(), df);
}

}  // namespace piol
}  // namespace exseis
