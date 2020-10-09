#ifndef EXSEISDAT_SRC_EXSEISDAT_PIOL_FILE_DETAIL_FILE_SEGY_IMPL_HH
#define EXSEISDAT_SRC_EXSEISDAT_PIOL_FILE_DETAIL_FILE_SEGY_IMPL_HH


#include "exseisdat/utils/encoding/number_encoding.hh"
#include "exseisdat/utils/types/typedefs.hh"

#include "exseisdat/piol/metadata/rules/Segy_rule_entry.hh"
#include "exseisdat/piol/segy/Trace_header_offsets.hh"
#include "exseisdat/piol/segy/utils.hh"

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace exseis {
namespace piol {
inline namespace file {
namespace detail {

class File_segy_impl {
  public:
    static size_t get_file_size(const IO_driver& io_driver);

    static void set_file_size(IO_driver& io_driver, size_t size);

    static void read_ho(const IO_driver& io_driver, unsigned char* buffer);

    static void should_write_file_header(
        IO_driver& io_driver, const unsigned char* buffer);

    static void read_trace_metadata(
        const IO_driver& io_driver,
        size_t trace_offset,
        size_t samples_per_trace,
        size_t number_of_traces,
        unsigned char* buffer);

    static void write_trace_metadata(
        IO_driver& io_driver,
        size_t trace_offset,
        size_t samples_per_trace,
        size_t number_of_traces,
        const unsigned char* buffer);

    static void read_trace_data(
        const IO_driver& io_driver,
        size_t trace_offset,
        size_t samples_per_trace,
        size_t number_of_traces,
        unsigned char* buffer);

    static void write_trace_data(
        IO_driver& io_driver,
        size_t trace_offset,
        size_t samples_per_trace,
        size_t number_of_traces,
        const unsigned char* buffer);

    static void read_trace(
        const IO_driver& io_driver,
        size_t trace_offset,
        size_t samples_per_trace,
        size_t number_of_traces,
        unsigned char* buffer);

    static void write_trace(
        IO_driver& io_driver,
        size_t trace_offset,
        size_t samples_per_trace,
        size_t number_of_traces,
        const unsigned char* buffer);

    static void read_trace(
        const IO_driver& io_driver,
        const size_t* trace_offsets,
        size_t samples_per_trace,
        size_t number_of_offsets,
        unsigned char* buffer);

    static void write_trace(
        IO_driver& io_driver,
        const size_t* trace_offsets,
        size_t samples_per_trace,
        size_t number_of_offsets,
        const unsigned char* buffer);

    static void read_trace_metadata(
        const IO_driver& io_driver,
        const size_t* trace_offsets,
        size_t samples_per_trace,
        size_t number_of_offsets,
        unsigned char* buffer);

    static void write_trace_metadata(
        IO_driver& io_driver,
        const size_t* trace_offsets,
        size_t samples_per_trace,
        size_t number_of_offsets,
        const unsigned char* buffer);

    static void read_trace_data(
        const IO_driver& io_driver,
        const size_t* trace_offsets,
        size_t samples_per_trace,
        size_t number_of_offsets,
        unsigned char* buffer);

    static void write_trace_data(
        IO_driver& io_driver,
        const size_t* trace_offsets,
        size_t samples_per_trace,
        size_t number_of_offsets,
        const unsigned char* buffer);

    static void extract_trace_metadata(
        size_t number_of_traces,
        const unsigned char* buffer,
        Trace_metadata* trace_metadata,
        size_t stride,
        size_t skip);

    static void insert_trace_metadata(
        size_t number_of_traces,
        const Trace_metadata* trace_metadata,
        unsigned char* buffer,
        size_t stride,
        size_t skip);
};


//////////////////////      Constructor & Destructor      //////////////////////
inline size_t File_segy_impl::get_file_size(const IO_driver& io_driver)
{
    return io_driver.get_file_size();
}

inline void File_segy_impl::set_file_size(IO_driver& io_driver, size_t size)
{
    return io_driver.set_file_size(size);
}

inline void File_segy_impl::read_ho(
    const IO_driver& io_driver, unsigned char* buffer)
{
    io_driver.read(0LU, segy::segy_binary_file_header_size(), buffer);
}

inline void File_segy_impl::should_write_file_header(
    IO_driver& io_driver, const unsigned char* buffer)
{
    if (buffer != nullptr) {
        io_driver.write(0LU, segy::segy_binary_file_header_size(), buffer);
    }
    else {
        io_driver.write(0LU, 0U, buffer);
    }
}

inline void File_segy_impl::read_trace(
    const IO_driver& io_driver,
    size_t trace_offset,
    size_t samples_per_trace,
    size_t number_of_traces,
    unsigned char* buffer)
{
    io_driver.read(
        segy::segy_trace_location(trace_offset, samples_per_trace),
        number_of_traces * segy::segy_trace_size(samples_per_trace), buffer);
}

inline void File_segy_impl::write_trace(
    IO_driver& io_driver,
    size_t trace_offset,
    size_t samples_per_trace,
    size_t number_of_traces,
    const unsigned char* buffer)
{
    io_driver.write(
        segy::segy_trace_location(trace_offset, samples_per_trace),
        number_of_traces * segy::segy_trace_size(samples_per_trace), buffer);
}

inline void File_segy_impl::read_trace_metadata(
    const IO_driver& io_driver,
    size_t trace_offset,
    size_t samples_per_trace,
    size_t number_of_traces,
    unsigned char* buffer)
{
    io_driver.read_strided(
        segy::segy_trace_location(trace_offset, samples_per_trace),
        segy::segy_trace_header_size(),
        segy::segy_trace_size(samples_per_trace), number_of_traces, buffer);
}

inline void File_segy_impl::write_trace_metadata(
    IO_driver& io_driver,
    size_t trace_offset,
    size_t samples_per_trace,
    size_t number_of_traces,
    const unsigned char* buffer)
{
    io_driver.write_strided(
        segy::segy_trace_location(trace_offset, samples_per_trace),
        segy::segy_trace_header_size(),
        segy::segy_trace_size(samples_per_trace), number_of_traces, buffer);
}

inline void File_segy_impl::read_trace_data(
    const IO_driver& io_driver,
    size_t trace_offset,
    size_t samples_per_trace,
    size_t number_of_traces,
    unsigned char* buffer)
{
    io_driver.read_strided(
        segy::segy_trace_data_location(trace_offset, samples_per_trace),
        segy::segy_trace_data_size(samples_per_trace),
        segy::segy_trace_size(samples_per_trace), number_of_traces, buffer);
}

inline void File_segy_impl::write_trace_data(
    IO_driver& io_driver,
    size_t trace_offset,
    size_t samples_per_trace,
    size_t number_of_traces,
    const unsigned char* buffer)
{
    io_driver.write_strided(
        segy::segy_trace_data_location(trace_offset, samples_per_trace),
        segy::segy_trace_data_size(samples_per_trace),
        segy::segy_trace_size(samples_per_trace), number_of_traces, buffer);
}

// TODO: Add optional validation in this layer?
inline void File_segy_impl::read_trace(
    const IO_driver& io_driver,
    const size_t* trace_offsets,
    size_t samples_per_trace,
    size_t number_of_offsets,
    unsigned char* buffer)
{
    std::vector<size_t> dooff(number_of_offsets);

    for (size_t i = 0; i < number_of_offsets; i++) {
        dooff[i] =
            segy::segy_trace_location(trace_offsets[i], samples_per_trace);
    }

    io_driver.read_offsets(
        segy::segy_trace_size(samples_per_trace), number_of_offsets,
        dooff.data(), buffer);
}

inline void File_segy_impl::write_trace(
    IO_driver& io_driver,
    const size_t* trace_offsets,
    size_t samples_per_trace,
    size_t number_of_offsets,
    const unsigned char* buffer)
{
    std::vector<size_t> dooff(number_of_offsets);

    for (size_t i = 0; i < number_of_offsets; i++) {
        dooff[i] =
            segy::segy_trace_location(trace_offsets[i], samples_per_trace);
    }

    io_driver.write_offsets(
        segy::segy_trace_size(samples_per_trace), number_of_offsets,
        dooff.data(), buffer);
}

inline void File_segy_impl::read_trace_metadata(
    const IO_driver& io_driver,
    const size_t* trace_offsets,
    size_t samples_per_trace,
    size_t number_of_offsets,
    unsigned char* buffer)
{
    std::vector<size_t> dooff(number_of_offsets);

    for (size_t i = 0; i < number_of_offsets; i++) {
        dooff[i] =
            segy::segy_trace_location(trace_offsets[i], samples_per_trace);
    }

    io_driver.read_offsets(
        segy::segy_trace_header_size(), number_of_offsets, dooff.data(),
        buffer);
}

inline void File_segy_impl::write_trace_metadata(
    IO_driver& io_driver,
    const size_t* trace_offsets,
    size_t samples_per_trace,
    size_t number_of_offsets,
    const unsigned char* buffer)
{
    std::vector<size_t> dooff(number_of_offsets);

    for (size_t i = 0; i < number_of_offsets; i++) {
        dooff[i] =
            segy::segy_trace_location(trace_offsets[i], samples_per_trace);
    }

    io_driver.write_offsets(
        segy::segy_trace_header_size(), number_of_offsets, dooff.data(),
        buffer);
}

inline void File_segy_impl::read_trace_data(
    const IO_driver& io_driver,
    const size_t* trace_offsets,
    size_t samples_per_trace,
    size_t number_of_offsets,
    unsigned char* buffer)
{
    if (samples_per_trace == 0) {
        return;
    }

    std::vector<size_t> dooff(number_of_offsets);

    for (size_t i = 0; i < number_of_offsets; i++) {
        dooff[i] =
            segy::segy_trace_data_location(trace_offsets[i], samples_per_trace);
    }

    io_driver.read_offsets(
        segy::segy_trace_data_size(samples_per_trace), number_of_offsets,
        dooff.data(), buffer);
}

inline void File_segy_impl::write_trace_data(
    IO_driver& io_driver,
    const size_t* trace_offsets,
    size_t samples_per_trace,
    size_t number_of_offsets,
    const unsigned char* buffer)
{
    if (samples_per_trace == 0) {
        return;
    }

    std::vector<size_t> dooff(number_of_offsets);

    for (size_t i = 0; i < number_of_offsets; i++) {
        dooff[i] =
            segy::segy_trace_data_location(trace_offsets[i], samples_per_trace);
    }

    io_driver.write_offsets(
        segy::segy_trace_data_size(samples_per_trace), number_of_offsets,
        dooff.data(), buffer);
}

inline void File_segy_impl::insert_trace_metadata(
    size_t number_of_traces,
    const Trace_metadata* trace_metadata,
    unsigned char* buffer,
    size_t stride,
    size_t skip)
{
    if (number_of_traces == 0) return;

    const auto& r = trace_metadata->rules;
    size_t start  = r.start;

    // If a copy of the original metadata is held, write this before the
    // specifically parsed/modified metadata.
    if (r.num_copy != 0) {
        if (stride == 0) {
            std::copy(
                trace_metadata->raw_metadata.begin()
                    + skip * segy::segy_trace_header_size(),
                trace_metadata->raw_metadata.begin()
                    + (skip + number_of_traces)
                          * segy::segy_trace_header_size(),
                buffer);
        }
        else {
            for (size_t i = 0; i < number_of_traces; i++) {
                std::copy(
                    &trace_metadata->raw_metadata
                         [(i + skip) * segy::segy_trace_header_size()],
                    &trace_metadata->raw_metadata
                         [(skip + i + 1LU) * segy::segy_trace_header_size()],
                    &buffer[i * (stride + segy::segy_trace_header_size())]);
            }
        }
    }

    // Loop through the traces, writing the stored metadata.
    for (size_t i = 0; i < number_of_traces; i++) {

        // Get the position in the metadata buffer where the current trace
        // metadata starts.
        unsigned char* metadata_buffer = &buffer[(r.extent() + stride) * i];

        // Keep a list of entries that need to be scaled for the SEGY format.
        std::unordered_map<
            segy::Trace_header_offsets, int16_t,
            exseis::utils::Enum_hash<segy::Trace_header_offsets>>
            scal;

        std::vector<std::pair<Trace_metadata_key, const Segy_float_rule_entry*>>
            floating_point_rules;

        for (const auto& v : r.rule_entry_map) {

            const auto& entry = v.first;
            const auto& t     = v.second;
            const size_t loc  = t->loc - start - 1LU;

            switch (t->type()) {

                case Rule_entry::MdType::Float: {

                    floating_point_rules.push_back(
                        {entry, dynamic_cast<Segy_float_rule_entry*>(t.get())});

                    auto tr = static_cast<segy::Trace_header_offsets>(
                        floating_point_rules.back().second->scalar_location);

                    const int16_t scal1 =
                        (scal.find(tr) != scal.end() ? scal[tr] : 1);
                    const int16_t scal2 = segy::find_scalar(
                        trace_metadata->get_floating_point(i + skip, entry));

                    // if the scale is bigger than 1 that means we need to use
                    // the largest to ensure conservation of the most
                    // significant  digit otherwise we choose the scale that
                    // preserves the  most digits after the decimal place.
                    scal[tr] =
                        ((scal1 > 1 || scal2 > 1) ? std::max(scal1, scal2) :
                                                    std::min(scal1, scal2));

                } break;

                case Rule_entry::MdType::Short: {

                    const std::array<unsigned char, 2> be_short =
                        utils::to_big_endian<int16_t>(int16_t(
                            trace_metadata->get_integer(i + skip, entry)));

                    std::copy(
                        std::begin(be_short), std::end(be_short),
                        &metadata_buffer[loc]);

                } break;

                case Rule_entry::MdType::Long: {

                    const std::array<unsigned char, 4> be_long =
                        utils::to_big_endian<int32_t>(int32_t(
                            trace_metadata->get_integer(i + skip, entry)));

                    std::copy(
                        std::begin(be_long), std::end(be_long),
                        &metadata_buffer[loc]);

                } break;

                case Rule_entry::MdType::Index:
                    // Index rules aren't stored on disk.
                    break;

                case Rule_entry::MdType::Copy:
                    // The Copy rule has been handled above.
                    break;
            }
        }

        // Finish off the floats. Floats are inherently annoying in SEG-Y
        for (const auto& s : scal) {
            const auto be = utils::to_big_endian(s.second);

            std::copy(
                std::begin(be), std::end(be),
                &metadata_buffer[size_t(s.first) - start - 1LU]);
        }

        for (size_t j = 0; j < floating_point_rules.size(); j++) {
            exseis::utils::Floating_point gscale =
                segy::parse_scalar(scal[static_cast<segy::Trace_header_offsets>(
                    floating_point_rules[j].second->scalar_location)]);

            const auto be = utils::to_big_endian(int32_t(std::lround(
                trace_metadata->get_floating_point(
                    i + skip, floating_point_rules[j].first)
                / gscale)));

            std::copy(
                std::begin(be), std::end(be),
                &metadata_buffer
                    [floating_point_rules[j].second->loc - start - 1LU]);
        }
    }
}

inline void File_segy_impl::extract_trace_metadata(
    size_t number_of_traces,
    const unsigned char* buffer,
    Trace_metadata* trace_metadata,
    size_t stride,
    size_t skip)
{
    if (number_of_traces == 0) return;

    const Rule& r = trace_metadata->rules;

    // If a copy of the metadata is required, read this first.
    if (r.num_copy != 0) {
        if (stride == 0) {
            std::copy(
                buffer,
                &buffer[number_of_traces * segy::segy_trace_header_size()],
                &trace_metadata
                     ->raw_metadata[skip * segy::segy_trace_header_size()]);
        }
        else {
            // The size of the trace metadata in bytes
            const size_t metadata_size = segy::segy_trace_header_size();
            for (size_t i = 0; i < number_of_traces; i++) {
                std::copy(
                    &buffer[i * (stride + metadata_size)],
                    &buffer[i * (stride + metadata_size) + metadata_size],
                    &trace_metadata->raw_metadata[(i + skip) * metadata_size]);
            }
        }
    }

    // For each trace, read the specified metadata
    for (size_t i = 0; i < number_of_traces; i++) {

        // Get the starting point in the metadata buffer for the current trace
        const unsigned char* metadata_buffer =
            &buffer[(r.extent() + stride) * i];

        // Loop through each rule and extract data
        for (const auto& v : r.rule_entry_map) {

            const auto& entry = v.first;
            const auto& t     = v.second;
            size_t loc        = t->loc - r.start - 1LU;

            switch (t->type()) {
                case Rule_entry::MdType::Float: {

                    const size_t scalar_offset =
                        dynamic_cast<Segy_float_rule_entry*>(t.get())
                            ->scalar_location
                        - r.start - 1LU;

                    const auto parsed_scalar =
                        segy::parse_scalar(utils::from_big_endian<int16_t>(
                            metadata_buffer[scalar_offset + 0LU],
                            metadata_buffer[scalar_offset + 1LU]));

                    const auto unscaled_value = utils::from_big_endian<int32_t>(
                        metadata_buffer[loc + 0LU], metadata_buffer[loc + 1LU],
                        metadata_buffer[loc + 2LU], metadata_buffer[loc + 3LU]);


                    using F_type = exseis::utils::Floating_point;

                    const auto value =
                        parsed_scalar * static_cast<F_type>(unscaled_value);

                    trace_metadata->set_floating_point(i + skip, entry, value);

                } break;

                case Rule_entry::MdType::Short: {

                    const auto value = utils::from_big_endian<int16_t>(
                        metadata_buffer[loc + 0LU], metadata_buffer[loc + 1LU]);

                    trace_metadata->set_integer(i + skip, entry, value);

                } break;

                case Rule_entry::MdType::Long: {

                    const auto value = utils::from_big_endian<int32_t>(
                        metadata_buffer[loc + 0LU], metadata_buffer[loc + 1LU],
                        metadata_buffer[loc + 2LU], metadata_buffer[loc + 3LU]);

                    trace_metadata->set_integer(i + skip, entry, value);

                } break;

                case Rule_entry::MdType::Index:
                    // Index types aren't stored in the file.
                    break;

                case Rule_entry::MdType::Copy:
                    // The Copy rule has already been handled above.
                    break;
            }
        }
    }
}

}  // namespace detail
}  // namespace file
}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_SRC_EXSEISDAT_PIOL_FILE_DETAIL_FILE_SEGY_IMPL_HH
