#ifndef EXSEIS_SRC_EXSEIS_PIOL_FILE_DETAIL_FILE_SEGY_IMPL_HH
#define EXSEIS_SRC_EXSEIS_PIOL_FILE_DETAIL_FILE_SEGY_IMPL_HH


#include "exseis/piol/file/detail/Blob_parser_segy.hh"
#include "exseis/piol/segy/Trace_header_offsets.hh"
#include "exseis/piol/segy/utils.hh"
#include "exseis/utils/encoding/number_encoding.hh"
#include "exseis/utils/types/typedefs.hh"

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <map>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace exseis {
inline namespace piol {
inline namespace file {
namespace detail {

class File_segy_impl {
  public:
    static std::vector<Trace_metadata_key> trace_metadata_keys();

    static std::map<size_t, std::unique_ptr<Blob_parser>>
    trace_metadata_parsers();

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
};


inline std::vector<Trace_metadata_key> File_segy_impl::trace_metadata_keys()
{
    using Key = Trace_metadata_key;
    std::vector<Trace_metadata_key> value;

#define EXSEIS_DETAIL_MAKE_TRACE_METADATA_LIST(ENUM, DESC)                     \
    value.push_back(Key::ENUM);

    EXSEIS_X_TRACE_METADATA_KEYS(EXSEIS_DETAIL_MAKE_TRACE_METADATA_LIST)

#undef EXSEIS_DETAIL_MAKE_TRACE_METADATA_LIST

    return value;
}

inline std::map<size_t, std::unique_ptr<Blob_parser>>
File_segy_impl::trace_metadata_parsers()
{
    std::map<size_t, std::unique_ptr<Blob_parser>> value;

    for (auto key : trace_metadata_keys()) {
        auto blob_parser = detail::make_blob_parser_segy(key);
        if (blob_parser) {
            value.emplace(std::make_pair(
                static_cast<size_t>(key), std::move(blob_parser)));
        }
    }

    return value;
}

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

}  // namespace detail
}  // namespace file
}  // namespace piol
}  // namespace exseis

#endif  // EXSEIS_SRC_EXSEIS_PIOL_FILE_DETAIL_FILE_SEGY_IMPL_HH
