#include "exseis/piol/parser/Blob_parser.hh"
#include <iostream>

#include "exseis/test/catch2.hh"

#include "exseis/test/segy/Segy_file_generator.hh"

#include "exseis/piol/segy/Trace_header_offsets.hh"
#include "exseis/utils/encoding/number_encoding.hh"

#include <cstdint>
#include <vector>


namespace {

// Parse the x_src value from a SEGY Trace.
// The x_src location needs to read both the coordinate scaling value
// and the coordinate location itself
class Blob_parser_x_src : public exseis::Blob_parser {
    static constexpr size_t coordinate_scalar_begin()
    {
        // -1 because Trace_header_offsets are 1-indexed
        return static_cast<size_t>(
                   exseis::segy::Trace_header_offsets::coordinate_scalar)
               - 1;
    }
    static constexpr size_t coordinate_scalar_end()
    {
        return coordinate_scalar_begin() + sizeof(int16_t);
    }

    static constexpr size_t x_src_begin()
    {
        // -1 because Trace_header_offsets are 1-indexed
        return static_cast<size_t>(exseis::segy::Trace_header_offsets::source_x)
               - 1;
    }

    static constexpr size_t x_src_end()
    {
        return x_src_begin() + sizeof(int32_t);
    }

    template<typename Iterator>
    static Iterator data_locations_impl(Iterator begin, Iterator end)
    {
        REQUIRE(std::distance(begin, end) >= 2);

        // Set coordinate scalar locations
        *begin = {coordinate_scalar_begin(), coordinate_scalar_end(), nullptr};

        ++begin;

        // Set x_src locations
        *begin = {x_src_begin(), x_src_end(), nullptr};

        ++begin;
        return begin;
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

    exseis::Parsed_type parsed_type() const override
    {
        return {exseis::Type::Double, 1};
    }

    void read(
        std::vector<exseis::Data_read_location>::const_iterator begin,
        std::vector<exseis::Data_read_location>::const_iterator end,
        void* value) const override

    {
        exseis::Data_read_location coordinate_scalar;
        exseis::Data_read_location x_src;

        for (auto it = begin; it != end; ++it) {
            // Check for coordinate_scalar
            if (it->begin == coordinate_scalar_begin()
                && it->end == coordinate_scalar_end()) {
                coordinate_scalar = *it;
            }

            // Check for x_src
            if (it->begin == x_src_begin() && it->end == x_src_end()) {
                x_src = *it;
            }
        }

        REQUIRE(coordinate_scalar.begin == coordinate_scalar_begin());
        REQUIRE(coordinate_scalar.end == coordinate_scalar_end());
        REQUIRE(coordinate_scalar.data != nullptr);

        REQUIRE(x_src.begin == x_src_begin());
        REQUIRE(x_src.end == x_src_end());
        REQUIRE(x_src.data != nullptr);

        const unsigned char* coordinate_scalar_data =
            static_cast<const unsigned char*>(coordinate_scalar.data);
        auto coordinate_scalar_native = exseis::from_big_endian<int16_t>(
            coordinate_scalar_data[0], coordinate_scalar_data[1]);

        const unsigned char* x_src_data =
            static_cast<const unsigned char*>(x_src.data);
        auto x_src_native = exseis::from_big_endian<int32_t>(
            x_src_data[0], x_src_data[1], x_src_data[2], x_src_data[3]);

        const double parsed_x_src =
            static_cast<double>(x_src_native)
            * static_cast<double>(coordinate_scalar_native);

        std::memcpy(value, &parsed_x_src, sizeof(double));
    }

    void write(
        std::vector<exseis::Data_write_location>::iterator begin,
        std::vector<exseis::Data_write_location>::iterator end,
        const void* value) const override
    {
        exseis::Data_write_location coordinate_scalar;
        exseis::Data_write_location x_src;

        for (auto it = begin; it != end; ++it) {
            if (it->begin == coordinate_scalar_begin()
                && it->end == coordinate_scalar_end()) {
                coordinate_scalar = *it;
            }

            if (it->begin == x_src_begin() && it->end == x_src_end()) {
                x_src = *it;
            }
        }

        // Get coordinate_scalar_native from the coordinate scalar data
        const unsigned char* coordinate_scalar_data =
            static_cast<const unsigned char*>(coordinate_scalar.data);
        auto coordinate_scalar_parsed = exseis::from_big_endian<int16_t>(
            coordinate_scalar_data[0], coordinate_scalar_data[1]);

        const double coordinate_scalar_native = ([=] {
            if (coordinate_scalar_parsed > 0) {
                return static_cast<double>(coordinate_scalar_parsed);
            }

            if (coordinate_scalar_parsed < 0) {
                return 1 / static_cast<double>(-coordinate_scalar_parsed);
            }

            // coordinate_scalar_parsed == 0
            return 1.0;
        }());

        // Get x_src from value
        double x_src_native;
        std::memcpy(&x_src_native, value, sizeof(double));

        // Scale x_src with coordinate_scalar and convert to stored type
        int32_t x_src_parsable =
            static_cast<int32_t>(x_src_native * coordinate_scalar_native);

        // Convert x_src to big endian and write
        auto x_src_parsable_be = exseis::to_big_endian(x_src_parsable);
        std::memcpy(
            x_src.data, x_src_parsable_be.data(), x_src_parsable_be.size());
    }
};

}  // namespace


TEST_CASE("Blob_parser")
{
    int32_t trace_index = GENERATE(0, 1, 10);
    auto segy_trace     = exseis::test::segy::Trace{
        trace_index, 0, exseis::segy::Segy_number_format::IEEE_fp32};


    std::unique_ptr<exseis::Blob_parser> blob_parser_x_src =
        std::make_unique<Blob_parser_x_src>();

    size_t number_of_data_locations =
        blob_parser_x_src->number_of_data_locations();
    REQUIRE(number_of_data_locations == 2);

    std::vector<exseis::Data_read_location> data_read_locations(
        number_of_data_locations);

    {
        INFO("Blob_parser::data_read_locations");

        auto it = blob_parser_x_src->data_read_locations(
            data_read_locations.begin(), data_read_locations.end());
        REQUIRE(it == data_read_locations.end());

        REQUIRE(data_read_locations.size() == 2);

        // Check ScaleCoord
        const auto scale_coord_location = data_read_locations[0];
        REQUIRE(
            scale_coord_location.begin
            == static_cast<size_t>(
                   exseis::segy::Trace_header_offsets::coordinate_scalar)
                   - 1);
        REQUIRE(
            scale_coord_location.end
            == scale_coord_location.begin + sizeof(int16_t));
        REQUIRE(scale_coord_location.data == nullptr);

        // Check x_src
        const auto x_src_location = data_read_locations[1];
        REQUIRE(
            x_src_location.begin
            == static_cast<size_t>(exseis::segy::Trace_header_offsets::source_x)
                   - 1);
        REQUIRE(x_src_location.end == x_src_location.begin + sizeof(int32_t));
        REQUIRE(x_src_location.data == nullptr);


        // Check Blob_parser::data_locations() returns the same data
        const auto alt_data_locations =
            blob_parser_x_src->data_read_locations();
        REQUIRE(std::equal(
            alt_data_locations.cbegin(), alt_data_locations.cend(),
            data_read_locations.cbegin(), data_read_locations.cend()));
    }


    auto parsed_type = blob_parser_x_src->parsed_type();

    {
        INFO("Blob_parser::parsed_type()");

        REQUIRE(parsed_type.type == exseis::Type::Double);
        REQUIRE(parsed_type.count == 1);
    }


    const auto trace_binary = segy_trace.binary();
    const auto trace_native = segy_trace.native();

    {
        double src_x = 0;

        {
            INFO("Blob_parser::read()");

            // Set the data_locations to the expected locations in trace_binary
            for (auto& data_location : data_read_locations) {
                data_location.data = trace_binary.data() + data_location.begin;
            }

            blob_parser_x_src->read(
                data_read_locations.cbegin(), data_read_locations.cend(),
                &src_x);

            // Check bitwise equality (!)
            REQUIRE(
                exseis::to_big_endian(src_x)
                == exseis::to_big_endian(trace_native.src_x));

            assert(
                src_x < 0
                || src_x > 0 && "src_x needs to be non-zero for write test");
        }

        {
            INFO("Blob_parser::write()");

            constexpr size_t coordinate_scalar_begin =
                +static_cast<size_t>(
                    exseis::segy::Trace_header_offsets::coordinate_scalar)
                - 1;

            constexpr size_t x_src_begin =
                static_cast<size_t>(
                    exseis::segy::Trace_header_offsets::source_x)
                - 1;

            std::vector<unsigned char> blob(trace_binary.size());

            auto data_write_locations =
                blob_parser_x_src->data_write_locations();

            // Set the data_locations to the local blob
            for (auto& data_location : data_write_locations) {
                data_location.data = blob.data() + data_location.begin;
            }

            // Sneaky pre-write the coordinate_scalar to the blob
            const auto coordinate_scalar_be =
                exseis::to_big_endian(trace_native.coordinate_scalar);
            std::memcpy(
                blob.data() + coordinate_scalar_begin,
                coordinate_scalar_be.data(), coordinate_scalar_be.size());

            //// Set the coordinate_scalar data_location to trace_binary
            // for (auto& data_location : data_write_locations) {
            //    if (data_location.begin == coordinate_scalar_begin
            //        && data_location.end
            //               == coordinate_scalar_begin + sizeof(int16_t)) {
            //        data_location.data =
            //            trace_binary.data() + data_location.begin;
            //    }
            //}

            blob_parser_x_src->write(
                data_write_locations.begin(), data_write_locations.end(),
                &src_x);

            // Compare x_src locations in blob and trace_binary
            const unsigned char* blob_x_src = blob.data() + x_src_begin;
            const unsigned char* trace_binary_x_src =
                trace_binary.data() + x_src_begin;

            REQUIRE(std::equal(
                blob_x_src, blob_x_src + sizeof(int32_t), trace_binary_x_src,
                trace_binary_x_src + sizeof(int32_t)));
        }
    }
}
