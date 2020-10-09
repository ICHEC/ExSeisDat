////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date July 2016
/// @brief
/// @details Input_file_segy functions
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/piol/file/Input_file_segy.hh"

#include "exseisdat/piol/file/detail/File_segy_impl.hh"

#include "exseisdat/piol/io_driver/IO_driver_mpi.hh"

#include "exseisdat/piol/operations/sort.hh"
#include "exseisdat/piol/segy/utils.hh"
#include "exseisdat/utils/encoding/character_encoding.hh"
#include "exseisdat/utils/encoding/number_encoding.hh"

#include <algorithm>
#include <cassert>

/// TODO: Fix me! Explicit reference to IO_driver_mpi should be removed.
#include <mpi.h>

using namespace exseis::utils;

namespace exseis {
namespace piol {
inline namespace file {

//////////////////////      Constructor & Destructor      //////////////////////

Input_file_segy::Implementation::Implementation(
    std::shared_ptr<ExSeisPIOL> piol,
    const std::string name,
    const Input_file_segy::Options& options) :
    Input_file_segy::Implementation::Implementation(
        IO_driver_mpi(name, File_mode_mpi::Read, MPI_COMM_WORLD, piol->log),
        piol,
        name,
        options)
{
}

Input_file_segy::Implementation::Implementation(
    IO_driver io_driver,
    std::shared_ptr<ExSeisPIOL> piol,
    std::string name,
    const Input_file_segy::Options& options) :
    m_piol(piol),
    m_name(name),
    m_io_driver(std::move(io_driver)),
    m_sample_interval_factor(options.sample_interval_factor)
{
    Input_file_segy::Implementation::read_file_headers();
}

const std::string& Input_file_segy::Implementation::file_name() const
{
    return m_name;
}

void Input_file_segy::Implementation::read_file_headers()
{
    using namespace segy;

    size_t file_header_size = segy::segy_binary_file_header_size();
    size_t file_size = detail::File_segy_impl::get_file_size(m_io_driver);

    // Read the global header data, if there is any.
    if (file_size >= file_header_size) {

        // Read the header into header_buffer
        auto header_buffer = std::vector<unsigned char>(file_header_size);
        detail::File_segy_impl::read_ho(m_io_driver, header_buffer.data());

        // Parse the number of samples, traces, the increment and the format
        // from header_buffer.
        m_samples_per_trace = static_cast<size_t>(from_big_endian<int16_t>(
            header_buffer[Segy_file_header_byte::num_sample + 0],
            header_buffer[Segy_file_header_byte::num_sample + 1]));

        m_number_of_traces =
            segy::get_number_of_traces(file_size, m_samples_per_trace);

        m_sample_interval =
            m_sample_interval_factor
            * exseis::utils::Floating_point(from_big_endian<int16_t>(
                header_buffer[Segy_file_header_byte::interval + 0],
                header_buffer[Segy_file_header_byte::interval + 1]));

        m_number_format =
            static_cast<Segy_number_format>(from_big_endian<int16_t>(
                header_buffer[Segy_file_header_byte::type + 0],
                header_buffer[Segy_file_header_byte::type + 1]));

        // Set this->m_text to the ASCII encoding of the text header data read
        // into header_buffer.
        // Determine if the current encoding is ASCII or EBCDIC from number of
        // printable ASCII or EBCDIC characters in the string.

        // Text header buffer bounds
        assert(header_buffer.size() >= segy::segy_text_header_size());
        const auto text_header_begin = std::begin(header_buffer);
        const auto text_header_end =
            text_header_begin + segy::segy_text_header_size();

        // Count printable ASCII
        const auto n_printable_ascii = std::count_if(
            text_header_begin, text_header_end, is_printable_ascii);

        // Count printable EBCDIC
        const auto n_printable_ebcdic = std::count_if(
            text_header_begin, text_header_end, is_printable_ebcdic);

        // Set text object to correct size in preparation for setting
        m_text.resize(segy::segy_text_header_size());

        if (n_printable_ascii > n_printable_ebcdic) {
            // The string is in ASCII, copy it.
            std::copy(text_header_begin, text_header_end, std::begin(m_text));
        }
        else {
            // The string is in EBCDIC, transform and copy it.
            std::transform(
                text_header_begin, text_header_end, std::begin(m_text),
                to_ascii_from_ebcdic);
        }
    }
}

size_t Input_file_segy::Implementation::read_number_of_traces() const
{
    return m_number_of_traces;
}

const std::string& Input_file_segy::Implementation::read_text() const
{
    return m_text;
}

size_t Input_file_segy::Implementation::read_samples_per_trace() const
{
    return m_samples_per_trace;
}

exseis::utils::Floating_point
Input_file_segy::Implementation::read_sample_interval() const
{
    return m_sample_interval;
}


namespace {

void convert_trace_data(
    segy::Segy_number_format m_number_format,
    size_t m_samples_per_trace,
    size_t number_of_traces,
    unsigned char* trace_data_buffer)
{
    // Convert trace values from SEGY floating point format to host format
    if (m_number_format == segy::Segy_number_format::IBM_fp32) {
        for (size_t i = 0; i < m_samples_per_trace * number_of_traces; i++) {
            unsigned char* trace_data_i =
                trace_data_buffer + i * sizeof(exseis::utils::Trace_value);

            std::array<unsigned char, 4> be_bytes_array = {
                {trace_data_i[0], trace_data_i[1], trace_data_i[2],
                 trace_data_i[3]}};

            exseis::utils::Trace_value trace_data =
                from_ibm_to_float(be_bytes_array, true);

            std::memcpy(trace_data_i, &trace_data, sizeof(trace_data));
        }
    }
    else {
        for (size_t i = 0; i < m_samples_per_trace * number_of_traces; i++) {
            unsigned char* trace_data_i =
                trace_data_buffer + i * sizeof(exseis::utils::Trace_value);

            exseis::utils::Trace_value trace_data =
                from_big_endian<exseis::utils::Trace_value>(
                    trace_data_i[0], trace_data_i[1], trace_data_i[2],
                    trace_data_i[3]);

            std::memcpy(
                trace_data_i, &trace_data, sizeof(exseis::utils::Trace_value));
        }
    }
}

template<typename T>
void read_trace_metadata_impl(
    const IO_driver& m_io_driver,
    size_t m_samples_per_trace,
    const T trace_offset,
    std::function<size_t(size_t)> offunc,
    size_t number_of_traces,
    Trace_metadata* trace_metadata,
    size_t skip)
{
    std::vector<unsigned char> buffer(
        segy::segy_trace_header_size() * number_of_traces);

    detail::File_segy_impl::read_trace_metadata(
        m_io_driver, trace_offset, m_samples_per_trace, number_of_traces,
        buffer.data());

    detail::File_segy_impl::extract_trace_metadata(
        number_of_traces, buffer.data(), trace_metadata, 0, skip);

    for (size_t i = 0; i < number_of_traces; i++) {
        trace_metadata->set_index(i + skip, Trace_metadata_key::ltn, offunc(i));
    }
}

template<typename T>
void read_trace_data_impl(
    const IO_driver& m_io_driver,
    segy::Segy_number_format m_number_format,
    size_t m_samples_per_trace,
    const T trace_offset,
    size_t number_of_traces,
    unsigned char* trace_data)
{
    detail::File_segy_impl::read_trace_data(
        m_io_driver, trace_offset, m_samples_per_trace, number_of_traces,
        trace_data);

    convert_trace_data(
        m_number_format, m_samples_per_trace, number_of_traces, trace_data);
}

template<typename T>
void read_trace_impl(
    const IO_driver& m_io_driver,
    segy::Segy_number_format m_number_format,
    size_t m_samples_per_trace,
    const T trace_offset,
    std::function<size_t(size_t)> offunc,
    size_t number_of_traces,
    unsigned char* trace_data,
    Trace_metadata* trace_metadata,
    size_t skip)
{
    std::vector<unsigned char> buffer(
        segy::segy_trace_size(m_samples_per_trace) * number_of_traces);

    detail::File_segy_impl::read_trace(
        m_io_driver, trace_offset, m_samples_per_trace, number_of_traces,
        buffer.data());

    for (size_t i = 0; i < number_of_traces; i++) {
        std::copy(
            &buffer
                [i * segy::segy_trace_size(m_samples_per_trace)
                 + segy::segy_trace_header_size()],
            &buffer[(i + 1) * segy::segy_trace_size(m_samples_per_trace)],
            &trace_data[i * segy::segy_trace_data_size(m_samples_per_trace)]);
    }

    detail::File_segy_impl::extract_trace_metadata(
        number_of_traces, buffer.data(), trace_metadata,
        segy::segy_trace_data_size(m_samples_per_trace), skip);

    for (size_t i = 0; i < number_of_traces; i++) {
        trace_metadata->set_index(i + skip, Trace_metadata_key::ltn, offunc(i));
    }

    convert_trace_data(
        m_number_format, m_samples_per_trace, number_of_traces, trace_data);
}

}  // namespace

#define EXSEISDAT_CHECK_NT(FUNCTION_NAME)                                      \
    do {                                                                       \
        if (number_of_traces > 0                                               \
            && trace_offset + number_of_traces > m_number_of_traces) {         \
            m_piol->log->add_entry(exseis::utils::Log_entry{                   \
                exseis::utils::Status::Warning,                                \
                "Number of traces to read out of range",                       \
                exseis::utils::Verbosity::none,                                \
                EXSEISDAT_SOURCE_POSITION(FUNCTION_NAME)});                    \
        }                                                                      \
    } while (false)

void Input_file_segy::Implementation::read_metadata(
    const size_t trace_offset,
    const size_t number_of_traces,
    Trace_metadata& trace_metadata,
    const size_t skip) const
{
    EXSEISDAT_CHECK_NT(
        "exseis::piol::Input_file_segy::Implementation::read_metadata");

    read_trace_metadata_impl(
        m_io_driver, m_samples_per_trace, trace_offset,
        [trace_offset](size_t i) -> size_t { return trace_offset + i; },
        number_of_traces, &trace_metadata, skip);
}

void Input_file_segy::Implementation::read_metadata() const
{
    read_trace_metadata_impl(
        m_io_driver, m_samples_per_trace, 0, {}, 0, nullptr, 0);
}

void Input_file_segy::Implementation::read_data(
    const size_t trace_offset,
    const size_t number_of_traces,
    Trace_value* trace_data) const
{
    EXSEISDAT_CHECK_NT(
        "exseis::piol::Input_file_segy::Implementation::read_data");

    read_trace_data_impl(
        m_io_driver, m_number_format, m_samples_per_trace, trace_offset,
        number_of_traces, reinterpret_cast<unsigned char*>(trace_data));
}

void Input_file_segy::Implementation::read_data() const
{
    read_trace_data_impl(
        m_io_driver, m_number_format, m_samples_per_trace, 0, 0, nullptr);
}

void Input_file_segy::Implementation::read(
    const size_t trace_offset,
    const size_t number_of_traces,
    exseis::utils::Trace_value* trace_data,
    Trace_metadata& trace_metadata,
    const size_t skip) const
{
    EXSEISDAT_CHECK_NT("exseis::piol::Input_file_segy::Implementation::read");

    read_trace_impl(
        m_io_driver, m_number_format, m_samples_per_trace, trace_offset,
        [trace_offset](size_t i) -> size_t { return trace_offset + i; },
        number_of_traces, reinterpret_cast<unsigned char*>(trace_data),
        &trace_metadata, skip);
}

void Input_file_segy::Implementation::read() const
{
    read_trace_impl(
        m_io_driver, m_number_format, m_samples_per_trace, size_t(0), {}, 0,
        nullptr, nullptr, 0);
}


void Input_file_segy::Implementation::read_metadata_non_contiguous(
    const size_t number_of_offsets,
    const size_t* trace_offsets,
    Trace_metadata& trace_metadata,
    const size_t skip) const
{
    read_trace_metadata_impl(
        m_io_driver, m_samples_per_trace, trace_offsets,
        [&trace_offsets](size_t i) -> size_t { return trace_offsets[i]; },
        number_of_offsets, &trace_metadata, skip);
}

void Input_file_segy::Implementation::read_metadata_non_contiguous() const
{
    read_trace_metadata_impl(
        m_io_driver, m_samples_per_trace, nullptr, {}, 0, nullptr, 0);
}

void Input_file_segy::Implementation::read_data_non_contiguous(
    const size_t number_of_offsets,
    const size_t* trace_offsets,
    Trace_value* trace_data) const
{
    read_trace_data_impl(
        m_io_driver, m_number_format, m_samples_per_trace, trace_offsets,
        number_of_offsets, reinterpret_cast<unsigned char*>(trace_data));
}

void Input_file_segy::Implementation::read_data_non_contiguous() const
{
    read_trace_data_impl(
        m_io_driver, m_number_format, m_samples_per_trace, nullptr, 0, nullptr);
}

void Input_file_segy::Implementation::read_non_contiguous(
    const size_t number_of_offsets,
    const size_t* trace_offsets,
    exseis::utils::Trace_value* trace_data,
    Trace_metadata& trace_metadata,
    const size_t skip) const
{
    read_trace_impl(
        m_io_driver, m_number_format, m_samples_per_trace, trace_offsets,
        [&trace_offsets](size_t i) -> size_t { return trace_offsets[i]; },
        number_of_offsets, reinterpret_cast<unsigned char*>(trace_data),
        &trace_metadata, skip);
}

void Input_file_segy::Implementation::read_non_contiguous() const
{
    read_trace_impl(
        m_io_driver, m_number_format, m_samples_per_trace, nullptr, {}, 0,
        nullptr, nullptr, 0);
}

void Input_file_segy::Implementation::read_non_monotonic(
    const size_t number_of_offsets,
    const size_t* trace_offsets,
    exseis::utils::Trace_value* trace_data,
    Trace_metadata& trace_metadata,
    const size_t skip) const
{
    // Sort the initial trace_offsets and make a new trace_offsets without
    // duplicates
    auto idx = get_sort_index(number_of_offsets, trace_offsets);
    std::vector<size_t> nodups;
    nodups.push_back(trace_offsets[idx[0]]);
    for (size_t j = 1; j < number_of_offsets; j++) {
        if (trace_offsets[idx[j - 1]] != trace_offsets[idx[j]]) {
            nodups.push_back(trace_offsets[idx[j]]);
        }
    }

    Trace_metadata tmp_trace_metadata(trace_metadata.rules, nodups.size());
    std::vector<exseis::utils::Trace_value> strc(
        m_samples_per_trace * nodups.size());

    read_trace_impl(
        m_io_driver, m_number_format, m_samples_per_trace, nodups.data(),
        [&trace_offsets](size_t i) -> size_t { return trace_offsets[i]; },
        nodups.size(), reinterpret_cast<unsigned char*>(strc.data()),
        &tmp_trace_metadata, 0);

    for (size_t n = 0, j = 0; j < number_of_offsets; ++j) {
        if (j != 0 && trace_offsets[idx[j - 1]] != trace_offsets[idx[j]]) {
            n++;
        }

        trace_metadata.copy_entries(skip + idx[j], tmp_trace_metadata, n);
    }

    for (size_t n = 0, j = 0; j < number_of_offsets; ++j) {
        if (j != 0 && trace_offsets[idx[j - 1]] != trace_offsets[idx[j]]) {
            n++;
        }

        for (size_t k = 0; k < m_samples_per_trace; k++) {
            trace_data[idx[j] * m_samples_per_trace + k] =
                strc[n * m_samples_per_trace + k];
        }
    }
}

void Input_file_segy::Implementation::read_non_monotonic() const
{
    read_trace_impl(
        m_io_driver, m_number_format, m_samples_per_trace, nullptr, {}, 0,
        nullptr, nullptr, 0);
}

std::vector<IO_driver> Input_file_segy::Implementation::io_drivers() &&
{
    std::vector<IO_driver> io_drivers;
    io_drivers.emplace_back(std::move(m_io_driver));
    return io_drivers;
}

}  // namespace file
}  // namespace piol
}  // namespace exseis
