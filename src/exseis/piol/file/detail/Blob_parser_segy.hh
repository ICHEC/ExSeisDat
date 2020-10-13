#ifndef EXSEIS_SRC_EXSEIS_PIOL_FILE_DETAIL_BLOB_PARSER_SEGY
#define EXSEIS_SRC_EXSEIS_PIOL_FILE_DETAIL_BLOB_PARSER_SEGY

#include "exseis/piol/parser/Blob_parser.hh"

#include "exseis/piol/metadata/Trace_metadata_key.hh"
#include "exseis/piol/segy/Trace_header_offsets.hh"
#include "exseis/piol/segy/utils.hh"
#include "exseis/utils/encoding/number_encoding.hh"
#include "exseis/utils/types/Type.hh"

#include <algorithm>
#include <type_traits>
#include <vector>

namespace exseis {
inline namespace piol {
inline namespace file {
namespace detail {

/// @brief Copy the full trace blob without parsing
class Blob_parser_segy_raw : public Blob_parser {

    template<typename Iterator>
    static Iterator data_locations_impl(Iterator begin, Iterator end)
    {
        assert(begin != end);
        (void)end;

        *begin = {0, segy::segy_trace_header_size(), nullptr};

        ++begin;
        return begin;
    }

    template<typename Iterator>
    static Iterator find_data_location(Iterator begin, Iterator end)
    {
        for (auto it = begin; it != end; ++it) {
            if (it->begin == 0 && it->end == segy::segy_trace_header_size()) {
                return it;
            }
        }
        return end;
    }

  public:
    size_t number_of_data_locations() const override { return 1; }

    std::vector<Data_read_location>::iterator data_read_locations(
        std::vector<Data_read_location>::iterator begin,
        std::vector<Data_read_location>::iterator end) const override
    {
        return data_locations_impl(begin, end);
    }

    std::vector<Data_write_location>::iterator data_write_locations(
        std::vector<Data_write_location>::iterator begin,
        std::vector<Data_write_location>::iterator end) const override
    {
        return data_locations_impl(begin, end);
    }

    Parsed_type parsed_type() const override
    {
        return {Type::UInt8, segy::segy_trace_header_size()};
    }

    void read(
        std::vector<Data_read_location>::const_iterator begin,
        std::vector<Data_read_location>::const_iterator end,
        void* value) const override
    {
        begin = find_data_location(begin, end);
        assert(begin != end);

        std::memcpy(value, begin->data, segy::segy_trace_header_size());
    }

    void write(
        std::vector<exseis::Data_write_location>::iterator begin,
        std::vector<exseis::Data_write_location>::iterator end,
        const void* value) const override
    {
        begin = find_data_location(begin, end);
        assert(begin != end);

        std::memcpy(begin->data, value, segy::segy_trace_header_size());
    }
};


/// @brief Parse a value at the given trace offset
template<typename T, segy::Trace_header_offsets Offset>
class Blob_parser_segy_single : public Blob_parser {
    static constexpr size_t offset_begin()
    {
        // -1 because Trace_header_offset is 1-indexed
        return static_cast<size_t>(Offset) - 1;
    }

    static constexpr size_t offset_end() { return offset_begin() + sizeof(T); }

    template<typename Iterator>
    static Iterator find_data_location(Iterator begin, Iterator end)
    {
        assert(std::is_sorted(
            begin, end, [](auto a, auto b) { return a.begin < b.begin; }));
        assert(begin != end);

        do {
            // Get the first element where begins match
            begin = std::lower_bound(
                begin, end, offset_begin(),
                [](auto e, size_t v) { return e.begin < v; });

            assert(begin != end);
            assert(begin->begin == offset_begin());

            // If end matches, great!
            // If not, tick along and try again
            if (begin->end == offset_end()) break;
            ++begin;
        } while (true);

        return begin;
    }

  public:
    size_t number_of_data_locations() const override { return 1; }

    template<typename Iterator>
    static Iterator data_locations_impl(Iterator begin, Iterator end)
    {
        assert(begin != end);
        (void)end;

        *begin = {offset_begin(), offset_end(), nullptr};

        ++begin;
        return begin;
    }

    std::vector<exseis::Data_read_location>::iterator data_read_locations(
        std::vector<exseis::Data_read_location>::iterator begin,
        std::vector<exseis::Data_read_location>::iterator end) const override
    {
        return data_locations_impl(begin, end);
    }

    std::vector<exseis::Data_write_location>::iterator data_write_locations(
        std::vector<exseis::Data_write_location>::iterator begin,
        std::vector<exseis::Data_write_location>::iterator end) const override
    {
        return data_locations_impl(begin, end);
    }

    Parsed_type parsed_type() const override { return {exseis::type<T>, 1}; }

    void read(
        std::vector<Data_read_location>::const_iterator begin,
        std::vector<Data_read_location>::const_iterator end,
        void* value) const override
    {
        read_impl(begin, end, value);
    }

    // NOTE: Expects Data_location list to be sorted by Data_location::begin.
    //       Also does no error checking. Expects the correct Data_location to
    //       be supplied
    template<typename Iterator>
    static void read_impl(Iterator begin, Iterator end, void* value)
    {
        begin = find_data_location(begin, end);

        const unsigned char* data =
            static_cast<const unsigned char*>(begin->data);

        // Workaround for not knowing indices for sizeof(T) in advance
        std::array<unsigned char, sizeof(T)> data_array;
        std::memcpy(data_array.data(), data, sizeof(T));
        const auto value_native = from_big_endian<T>(data_array);

        std::memcpy(value, &value_native, sizeof(T));
    }

    // NOTE: Expects Data_location list to be sorted by Data_location::begin.
    //       Also does no error checking. Expects the correct Data_location to
    //       be supplied
    void write(
        std::vector<exseis::Data_write_location>::iterator begin,
        std::vector<exseis::Data_write_location>::iterator end,
        const void* value) const override
    {
        write_impl(begin, end, value);
    }

    static void write_impl(
        std::vector<exseis::Data_write_location>::iterator begin,
        std::vector<exseis::Data_write_location>::iterator end,
        const void* value)
    {
        begin = find_data_location(begin, end);

        T value_native;
        std::memcpy(&value_native, value, sizeof(T));

        auto value_be = to_big_endian(value_native);

        std::memcpy(begin->data, value_be.data(), value_be.size());
    }
};

template<segy::Trace_header_offsets Coord, segy::Trace_header_offsets Scalar>
class Blob_parser_segy_scaled : public Blob_parser {

    using Coord_parser  = Blob_parser_segy_single<int32_t, Coord>;
    using Scalar_parser = Blob_parser_segy_single<int16_t, Scalar>;

    template<typename Iterator>
    static Iterator data_locations_impl(Iterator begin, Iterator end)
    {
        begin = Scalar_parser::data_locations_impl(begin, end);
        return Coord_parser::data_locations_impl(begin, end);
    }

    template<typename Iterator>
    double read_scalar(Iterator begin, Iterator end) const
    {
        int16_t scalar_raw;
        Scalar_parser::read_impl(begin, end, &scalar_raw);

        double scalar = 1.0;
        if (scalar_raw < 0) {
            scalar = 1 / static_cast<double>(-scalar_raw);
        }
        else if (scalar_raw > 0) {
            scalar = static_cast<double>(scalar_raw);
        }

        return scalar;
    }

  public:
    size_t number_of_data_locations() const override { return 2; }

    std::vector<exseis::Data_read_location>::iterator data_read_locations(
        std::vector<exseis::Data_read_location>::iterator begin,
        std::vector<exseis::Data_read_location>::iterator end) const override
    {
        return data_locations_impl(begin, end);
    }

    std::vector<exseis::Data_write_location>::iterator data_write_locations(
        std::vector<exseis::Data_write_location>::iterator begin,
        std::vector<exseis::Data_write_location>::iterator end) const override
    {
        return data_locations_impl(begin, end);
    }

    Parsed_type parsed_type() const override { return {Type::Double, 1}; }

    void read(
        std::vector<Data_read_location>::const_iterator begin,
        std::vector<Data_read_location>::const_iterator end,
        void* value) const override
    {
        double scalar = read_scalar(begin, end);

        int32_t coord_raw;
        Coord_parser::read_impl(begin, end, &coord_raw);

        const double coord = static_cast<double>(coord_raw) * scalar;

        std::memcpy(value, &coord, sizeof(double));
    }

    /// NOTE: Assumes the coordinate_scalar has already been written, and
    /// reads that to scale the input value
    void write(
        std::vector<exseis::Data_write_location>::iterator begin,
        std::vector<exseis::Data_write_location>::iterator end,
        const void* value) const override
    {
        double scalar = read_scalar(begin, end);

        double coord_native;
        std::memcpy(&coord_native, value, sizeof(double));

        const int32_t coord = static_cast<int32_t>(coord_native / scalar);

        Coord_parser::write_impl(begin, end, &coord);
    }
};

inline std::unique_ptr<Blob_parser> make_blob_parser_segy_single(
    segy::Trace_header_offsets offset)
{
#define EXSEIS_DETAIL_MAKE_BLOB_PARSER_SEGY_SINGLE(ENUM, OFFSET, TYPE, DESC)   \
    case segy::Trace_header_offsets::ENUM:                                     \
        return std::make_unique<Blob_parser_segy_single<                       \
            TYPE, segy::Trace_header_offsets::ENUM>>();

    switch (offset) {
        EXSEIS_X_SEGY_TRACE_HEADER_OFFSETS(
            EXSEIS_DETAIL_MAKE_BLOB_PARSER_SEGY_SINGLE);
    }

#undef EXSEIS_DETAIL_MAKE_BLOB_PARSER_SEGY_SINGLE

    return nullptr;
}

inline std::unique_ptr<Blob_parser> make_blob_parser_segy(
    Trace_metadata_key key)
{
#define EXSEIS_DETAIL_SINGLE(ENUM)                                             \
    case Trace_metadata_key::ENUM:                                             \
        return make_blob_parser_segy_single(segy::Trace_header_offsets::ENUM);

#define EXSEIS_DETAIL_SCALED(ENUM, SCALAR)                                     \
    case Trace_metadata_key::ENUM:                                             \
        return std::make_unique<Blob_parser_segy_scaled<                       \
            segy::Trace_header_offsets::ENUM,                                  \
            segy::Trace_header_offsets::SCALAR>>();

    switch (key) {
        case Trace_metadata_key::raw:
            return std::make_unique<Blob_parser_segy_raw>();

            EXSEIS_DETAIL_SINGLE(line_trace_index)
            EXSEIS_DETAIL_SINGLE(ofr_trace_index)
            EXSEIS_DETAIL_SINGLE(file_trace_index)
            EXSEIS_DETAIL_SINGLE(ensemble_trace_index)
            EXSEIS_DETAIL_SINGLE(number_of_samples)
            EXSEIS_DETAIL_SINGLE(sample_interval)
            EXSEIS_DETAIL_SINGLE(trace_id_code)
            EXSEIS_DETAIL_SINGLE(energy_source_number)

            EXSEIS_DETAIL_SINGLE(vstack_count)
            EXSEIS_DETAIL_SINGLE(hstack_count)
            EXSEIS_DETAIL_SINGLE(source_receiver_distance)

            EXSEIS_DETAIL_SCALED(water_depth_at_source, ScaleElev)
            EXSEIS_DETAIL_SCALED(water_depth_at_receiver, ScaleElev)
            EXSEIS_DETAIL_SCALED(receiver_elevation, ScaleElev)
            EXSEIS_DETAIL_SCALED(surface_elevation_at_source, ScaleElev)
            EXSEIS_DETAIL_SCALED(source_depth, ScaleElev)

            EXSEIS_DETAIL_SCALED(shotpoint_number, ShotScal)

            EXSEIS_DETAIL_SINGLE(coordinate_scalar)
            EXSEIS_DETAIL_SCALED(source_x, coordinate_scalar)
            EXSEIS_DETAIL_SCALED(source_y, coordinate_scalar)
            EXSEIS_DETAIL_SCALED(receiver_x, coordinate_scalar)
            EXSEIS_DETAIL_SCALED(receiver_y, coordinate_scalar)
            EXSEIS_DETAIL_SCALED(cdp_x, coordinate_scalar)
            EXSEIS_DETAIL_SCALED(cdp_y, coordinate_scalar)

            EXSEIS_DETAIL_SINGLE(il)
            EXSEIS_DETAIL_SINGLE(xl)
            EXSEIS_DETAIL_SINGLE(transduction_unit)
            EXSEIS_DETAIL_SINGLE(trace_value_unit)

        // Skipping
        case Trace_metadata_key::ltn:
            return nullptr;
        case Trace_metadata_key::gtn:
            return nullptr;
        case Trace_metadata_key::dsdr:
            return nullptr;
    }

#undef EXSEIS_DETAIL_SCALED
#undef EXSEIS_DETAIL_SINGLE

    return nullptr;
}

}  // namespace detail
}  // namespace file
}  // namespace piol
}  // namespace exseis

#endif  // EXSEIS_SRC_EXSEIS_PIOL_FILE_DETAIL_BLOB_PARSER_SEGY
