////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date July 2016
/// @brief
/// @details Output_file_segy functions
////////////////////////////////////////////////////////////////////////////////

#include "exseis/piol/file/Output_file_segy.hh"

#include "exseis/piol/file/detail/File_segy_impl.hh"

#include "exseis/piol/segy/utils.hh"
#include "exseis/utils/encoding/number_encoding.hh"

#include <cmath>
#include <cstring>
#include <limits>


namespace exseis {
inline namespace piol {
inline namespace file {

//////////////////////      Constructor & Destructor      //////////////////////

Output_file_segy::Implementation::Implementation(
    IO_driver io_driver, const Output_file_segy::Options& options) :
    m_io_driver(std::move(io_driver)),
    m_sample_interval_factor(options.sample_interval_factor),
    m_trace_header_parsers(detail::File_segy_impl::trace_metadata_parsers())
{
}

Output_file_segy::Implementation::~Implementation()
{
    // NOTE: Not calling virtual version of sync
    Output_file_segy::Implementation::sync();
}

std::string Output_file_segy::Implementation::file_name() const
{
    return m_io_driver.file_name();
}

//////////////////////////       Member functions      /////////////////////////

void Output_file_segy::Implementation::write_text(const std::string text)
{
    if (m_text != text) {
        m_text = text;
        m_text.resize(segy::segy_text_header_size());
        m_file_header_dirty = true;
    }
}

void Output_file_segy::Implementation::write_samples_per_trace(
    const size_t samples_per_trace)
{
    if (samples_per_trace > std::numeric_limits<int16_t>::max()) {
        m_io_driver.communicator().log()->add_entry(Log_entry{
            Status::Error, "Ns value is too large for SEG-Y", Verbosity::none,
            EXSEIS_SOURCE_POSITION(
                "exseis::piol::Output_file_segy::Implementation::write_samples_per_trace")});

        return;
    }

    // Ns shouldn't be set to a different value after samples_per_trace and
    // number_of_traces have already been set to non-zero values.
    if (m_samples_per_trace_set && m_samples_per_trace != samples_per_trace
        && m_samples_per_trace != 0 && m_number_of_traces != 0) {
        m_io_driver.communicator().log()->add_entry(Log_entry{
            Status::Error, "Ns cannot be set twice!", Verbosity::none,
            EXSEIS_SOURCE_POSITION(
                "exseis::piol::Output_file_segy::Implementation::write_samples_per_trace")});

        return;
    }

    if (!m_samples_per_trace_set || m_samples_per_trace != samples_per_trace) {
        m_samples_per_trace = samples_per_trace;

        m_samples_per_trace_set = true;
        m_size_dirty            = true;
    }
}

void Output_file_segy::Implementation::write_number_of_traces(
    const size_t number_of_traces)
{
    if (m_number_of_traces != number_of_traces) {
        m_number_of_traces = number_of_traces;

        m_size_dirty = true;
    }
}

void Output_file_segy::Implementation::write_sample_interval(
    const Floating_point sample_interval)
{
    if (!std::isnormal(sample_interval)) {
        m_io_driver.communicator().log()->add_entry(Log_entry{
            Status::Error,
            "The SEG-Y Interval " + std::to_string(sample_interval)
                + " is not normal.",
            Verbosity::none,
            EXSEIS_SOURCE_POSITION(
                "exseis::piol::Output_file_segy::Implementation::write_sample_interval")});

        return;
    }

    m_sample_interval   = sample_interval;
    m_file_header_dirty = true;
}

std::map<Trace_metadata_key, Trace_metadata_info>
Output_file_segy::Implementation::trace_metadata_available() const
{
    std::map<Trace_metadata_key, Trace_metadata_info> value;

    for (const auto& parser_it : m_trace_header_parsers) {
        const auto key          = parser_it.first;
        const auto& blob_parser = parser_it.second;

        const auto parsed_type = blob_parser->parsed_type();
        value[static_cast<Trace_metadata_key>(key)] =
            Trace_metadata_info{parsed_type.type, parsed_type.count};
    }

    return value;
}

namespace {
void insert_trace_metadata(
    const std::map<size_t, std::unique_ptr<Blob_parser>>&
        m_trace_header_parsers,
    size_t number_of_traces,
    const Trace_metadata* trace_metadata,
    unsigned char* buffer,
    size_t stride,
    size_t skip)
{
    if (number_of_traces == 0) return;

    // Initialize data_locations offsets
    std::vector<Data_write_location> data_locations;
    for (const auto& entry_type : trace_metadata->entry_types()) {
        Trace_metadata_key key = entry_type.first;

        const auto blob_parser_it =
            m_trace_header_parsers.find(static_cast<size_t>(key));
        if (blob_parser_it == m_trace_header_parsers.end()) continue;

        const auto& blob_parser = blob_parser_it->second;

        const size_t offset = data_locations.size();
        const size_t number_of_data_locations =
            blob_parser->number_of_data_locations();

        data_locations.resize(data_locations.size() + number_of_data_locations);

        blob_parser->data_write_locations(
            data_locations.begin() + offset, data_locations.end());
    }

    std::sort(
        data_locations.begin(), data_locations.end(),
        [](auto& a, auto& b) { return a.begin < b.begin; });

    // Loop over trace and parse
    for (size_t trace_index = 0; trace_index < number_of_traces;
         trace_index++) {
        unsigned char* trace_header = buffer + trace_index * stride;

        // Set the read locations in the trace_header
        for (auto& loc : data_locations) {
            loc.data = trace_header + loc.begin;
        }

        for (const auto& entry_type : trace_metadata->entry_types()) {
            Trace_metadata_key key = entry_type.first;
            auto type              = entry_type.second.type;
            auto count             = entry_type.second.count;

            const auto blob_parser_it =
                m_trace_header_parsers.find(static_cast<size_t>(key));
            if (blob_parser_it == m_trace_header_parsers.end()) continue;

            const auto& blob_parser = blob_parser_it->second;

            const auto parsed_type = blob_parser->parsed_type();

            assert(parsed_type.type == type);
            assert(parsed_type.count == count);

            auto tm_data_location =
                trace_metadata->get_data_read_location(trace_index + skip, key);

            blob_parser->write(
                data_locations.begin(), data_locations.end(),
                tm_data_location.data);
        }
    }
}

template<typename T>
void write_trace_metadata_impl(
    const std::map<size_t, std::unique_ptr<Blob_parser>>&
        m_trace_header_parsers,
    IO_driver& m_io_driver,
    size_t m_samples_per_trace,
    const T trace_offset,
    size_t number_of_traces,
    const Trace_metadata* trace_metadata,
    size_t skip)
{
    std::vector<unsigned char> buffer(
        segy::segy_trace_header_size() * number_of_traces);

    // detail::File_segy_impl::insert_trace_metadata(
    //    number_of_traces, trace_metadata, buffer.data(), 0, skip);
    insert_trace_metadata(
        m_trace_header_parsers, number_of_traces, trace_metadata, buffer.data(),
        segy::segy_trace_header_size(), skip);

    detail::File_segy_impl::write_trace_metadata(
        m_io_driver, trace_offset, m_samples_per_trace, number_of_traces,
        buffer.data());
}

template<typename T>
void write_trace_data_impl(
    IO_driver& m_io_driver,
    size_t m_samples_per_trace,
    const T trace_offset,
    size_t number_of_traces,
    const Trace_value* trace_data)
{
    std::vector<unsigned char> buffer(
        m_samples_per_trace * number_of_traces * sizeof(Trace_value));

    // Copy trace_data into buffer, converting from native-endian to big-endian
    // on the way.
    for (size_t sample_index = 0;
         sample_index < m_samples_per_trace * number_of_traces;
         sample_index++) {

        const auto input_sample_be = to_big_endian(trace_data[sample_index]);
        unsigned char* output_sample =
            buffer.data() + sample_index * sizeof(Trace_value);

        std::memcpy(
            output_sample, input_sample_be.data(), input_sample_be.size());
    }

    detail::File_segy_impl::write_trace_data(
        m_io_driver, trace_offset, m_samples_per_trace, number_of_traces,
        buffer.data());
}

template<typename T>
void write_trace_impl(
    const std::map<size_t, std::unique_ptr<Blob_parser>>&
        m_trace_header_parsers,
    IO_driver& m_io_driver,
    size_t m_samples_per_trace,
    const T trace_offset,
    size_t number_of_traces,
    const Trace_value* trace_data,
    const Trace_metadata* trace_metadata,
    size_t skip)
{
    std::vector<unsigned char> buffer(
        segy::segy_trace_size(m_samples_per_trace) * number_of_traces);

    // detail::File_segy_impl::insert_trace_metadata(
    //    number_of_traces, trace_metadata, buffer.data(),
    //    segy::segy_trace_data_size(m_samples_per_trace), skip);
    insert_trace_metadata(
        m_trace_header_parsers, number_of_traces, trace_metadata, buffer.data(),
        segy::segy_trace_size(m_samples_per_trace), skip);

    // Copy trace_data into the buffer, converting the native-endian trace_data
    // into big endian
    for (size_t trace_index = 0; trace_index < number_of_traces;
         trace_index++) {

        // Find the start of the input and output traces
        const Trace_value* input_trace =
            trace_data + trace_index * m_samples_per_trace;

        unsigned char* output_trace =
            buffer.data()
            + trace_index * segy::segy_trace_size(m_samples_per_trace)
            + segy::segy_trace_header_size();

        // Convert and copy each element
        for (size_t sample_index = 0; sample_index < m_samples_per_trace;
             sample_index++) {
            const auto input_sample_be =
                to_big_endian(input_trace[sample_index]);
            unsigned char* output_sample =
                output_trace + sample_index * sizeof(Trace_value);

            std::memcpy(
                output_sample, input_sample_be.data(), input_sample_be.size());
        }
    }

    detail::File_segy_impl::write_trace(
        m_io_driver, trace_offset, m_samples_per_trace, number_of_traces,
        buffer.data());
}

}  // namespace


#define EXSEIS_CHECK_NS(FUNCTION_NAME)                                         \
    do {                                                                       \
        if (!m_samples_per_trace_set) {                                        \
            m_io_driver.communicator().log()->add_entry(Log_entry{             \
                Status::Error,                                                 \
                "The number of samples per trace has not been set. "           \
                "The output is probably erroneous.",                           \
                Verbosity::none, EXSEIS_SOURCE_POSITION(FUNCTION_NAME)});      \
        }                                                                      \
    } while (false)

void Output_file_segy::Implementation::write_metadata(
    const size_t trace_offset,
    const size_t number_of_traces,
    const Trace_metadata& trace_metadata,
    const size_t skip)
{
    write_trace_metadata_impl(
        m_trace_header_parsers, m_io_driver, m_samples_per_trace, trace_offset,
        number_of_traces, &trace_metadata, skip);
    m_file_dirty = true;
}

void Output_file_segy::Implementation::write_metadata()
{
    write_trace_metadata_impl(
        m_trace_header_parsers, m_io_driver, m_samples_per_trace, 0, 0, nullptr,
        0);
    m_file_dirty = true;
}

void Output_file_segy::Implementation::write_data(
    const size_t trace_offset,
    const size_t number_of_traces,
    const Trace_value* trace_data)
{
    EXSEIS_CHECK_NS(
        "exseis::piol::Output_file_segy::Implementation::write_data");

    write_trace_data_impl(
        m_io_driver, m_samples_per_trace, trace_offset, number_of_traces,
        trace_data);
    m_file_dirty = true;
}

void Output_file_segy::Implementation::write_data()
{
    write_trace_data_impl(m_io_driver, m_samples_per_trace, 0, 0, nullptr);
    m_file_dirty = true;
}

void Output_file_segy::Implementation::write(
    const size_t trace_offset,
    const size_t number_of_traces,
    const Trace_value* trace_data,
    const Trace_metadata& trace_metadata,
    const size_t skip)
{
    EXSEIS_CHECK_NS("exseis::piol::Output_file_segy::Implementation::write");

    write_trace_impl(
        m_trace_header_parsers, m_io_driver, m_samples_per_trace, trace_offset,
        number_of_traces, trace_data, &trace_metadata, skip);
    m_file_dirty = true;
}

void Output_file_segy::Implementation::write()
{
    write_trace_impl(
        m_trace_header_parsers, m_io_driver, m_samples_per_trace, 0, 0, nullptr,
        nullptr, 0);
    m_file_dirty = true;
}

void Output_file_segy::Implementation::write_metadata_non_contiguous(
    const size_t number_of_offsets,
    const size_t* trace_offsets,
    const Trace_metadata& trace_metadata,
    const size_t skip)
{
    EXSEIS_CHECK_NS(
        "exseis::piol::Output_file_segy::Implementation::write_metadata_non_contiguous");

    write_trace_metadata_impl(
        m_trace_header_parsers, m_io_driver, m_samples_per_trace, trace_offsets,
        number_of_offsets, &trace_metadata, skip);
    m_file_dirty = true;
}

void Output_file_segy::Implementation::write_metadata_non_contiguous()
{
    write_trace_metadata_impl(
        m_trace_header_parsers, m_io_driver, m_samples_per_trace, 0, 0, nullptr,
        0);
    m_file_dirty = true;
}

void Output_file_segy::Implementation::write_data_non_contiguous(
    const size_t number_of_offsets,
    const size_t* trace_offsets,
    const Trace_value* trace_data)
{
    EXSEIS_CHECK_NS(
        "exseis::piol::Output_file_segy::Implementation::write_data_non_contiguous");

    write_trace_data_impl(
        m_io_driver, m_samples_per_trace, trace_offsets, number_of_offsets,
        trace_data);
    m_file_dirty = true;
}

void Output_file_segy::Implementation::write_data_non_contiguous()
{
    write_trace_data_impl(
        m_io_driver, m_samples_per_trace, nullptr, 0, nullptr);
    m_file_dirty = true;
}

void Output_file_segy::Implementation::write_non_contiguous(
    const size_t number_of_offsets,
    const size_t* trace_offsets,
    const Trace_value* trace_data,
    const Trace_metadata& trace_metadata,
    const size_t skip)
{
    EXSEIS_CHECK_NS(
        "exseis::piol::Output_file_segy::Implementation::write_non_contiguous");

    write_trace_impl(
        m_trace_header_parsers, m_io_driver, m_samples_per_trace, trace_offsets,
        number_of_offsets, trace_data, &trace_metadata, skip);
    m_file_dirty = true;
}

void Output_file_segy::Implementation::write_non_contiguous()
{
    write_trace_impl(
        m_trace_header_parsers, m_io_driver, m_samples_per_trace, nullptr, 0,
        nullptr, nullptr, 0);
    m_file_dirty = true;
}

std::vector<IO_driver> Output_file_segy::Implementation::io_drivers() &&
{
    sync();

    std::vector<IO_driver> io_drivers;
    io_drivers.emplace_back(std::move(m_io_driver));

    return io_drivers;
}

void Output_file_segy::Implementation::sync()
{
    // Write headers, resize file etc. etc.
    if (m_size_dirty) {
        detail::File_segy_impl::set_file_size(
            m_io_driver,
            segy::get_file_size(m_number_of_traces, m_samples_per_trace));

        // Update samples_per_trace, and sync
        m_file_header_dirty = true;
        m_file_dirty        = true;

        m_size_dirty = false;
    }

    if (m_file_header_dirty) {
        // Write file header on rank 0.
        if (m_io_driver.communicator().get_rank() == 0) {
            // The buffer to build the header in
            std::vector<unsigned char> header_buffer(
                segy::segy_binary_file_header_size());

            // Write the text header into the start of the buffer.
            std::copy(
                std::begin(m_text), std::end(m_text),
                std::begin(header_buffer));

            // Write samples_per_trace, the number format, and the interval
            const std::array<unsigned char, 2> be_samples_per_trace =
                to_big_endian(static_cast<int16_t>(m_samples_per_trace));
            std::copy(
                std::begin(be_samples_per_trace),
                std::end(be_samples_per_trace),
                &header_buffer[segy::Segy_file_header_byte::num_sample]);

            const std::array<unsigned char, 2> be_format = to_big_endian(
                static_cast<int16_t>(segy::Segy_number_format::IEEE_fp32));
            std::copy(
                std::begin(be_format), std::end(be_format),
                &header_buffer[segy::Segy_file_header_byte::type]);

            const std::array<unsigned char, 2> be_interval =
                to_big_endian(static_cast<int16_t>(
                    std::lround(m_sample_interval / m_sample_interval_factor)));
            std::copy(
                std::begin(be_interval), std::end(be_interval),
                &header_buffer[segy::Segy_file_header_byte::interval]);

            // Currently these are hard-coded entries:
            // The unit system.
            const std::array<unsigned char, 2> be_units =
                to_big_endian<int16_t>(0x0001);
            std::copy(
                std::begin(be_units), std::end(be_units),
                &header_buffer[segy::Segy_file_header_byte::units]);

            // The version of the SEGY format.
            const std::array<unsigned char, 2> be_segy_format =
                to_big_endian<int16_t>(0x0100);
            std::copy(
                std::begin(be_segy_format), std::end(be_segy_format),
                &header_buffer[segy::Segy_file_header_byte::segy_format]);

            // We always deal with fixed traces at present.
            const std::array<unsigned char, 2> be_fixed_trace =
                to_big_endian<int16_t>(0x0001);
            std::copy(
                std::begin(be_fixed_trace), std::end(be_fixed_trace),
                &header_buffer[segy::Segy_file_header_byte::fixed_trace]);

            // We do not support text extensions at present.
            const auto be_extensions = to_big_endian<int16_t>(0x0000);
            std::copy(
                std::begin(be_extensions), std::end(be_extensions),
                &header_buffer[segy::Segy_file_header_byte::extensions]);

            // Write the header from the buffer
            detail::File_segy_impl::should_write_file_header(
                m_io_driver, header_buffer.data());
        }
        else {
            detail::File_segy_impl::should_write_file_header(
                m_io_driver, nullptr);
        }

        // Sync file
        m_file_dirty = true;

        m_file_header_dirty = false;
    }

    // Sync to disk
    if (m_file_dirty) {
        m_io_driver.sync();
        m_file_dirty = false;
    }
}

}  // namespace file
}  // namespace piol
}  // namespace exseis
