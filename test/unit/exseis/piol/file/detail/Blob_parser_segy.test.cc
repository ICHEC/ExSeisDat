#include "exseis/piol/file/detail/Blob_parser_segy.hh"

#include "exseis/test/catch2.hh"

#include "exseis/piol/metadata/Trace_metadata_key.hh"
#include "exseis/test/segy/Segy_file_generator.hh"

#include <set>
#include <type_traits>
#include <vector>


std::set<exseis::Trace_metadata_key> all_trace_metadata_keys()
{
    std::set<exseis::Trace_metadata_key> value;

#define EXSEIS_MAKE_TRACE_METADATA_KEYS(ENUM, DESC)                            \
    value.insert(exseis::Trace_metadata_key::ENUM);
    EXSEIS_X_TRACE_METADATA_KEYS(EXSEIS_MAKE_TRACE_METADATA_KEYS)
#undef EXSEIS_MAKE_TRACE_METADATA_KEYS

    return value;
}

TEST_CASE("Blob_parser_segy", "[piol][file][detail][Blob_parser_segy]")
{
    SECTION ("Trace_metadata_key Coverage") {
        // Check coverage, without Copy, lt, gt
        auto trace_metadata_keys = all_trace_metadata_keys();

        trace_metadata_keys.erase(exseis::Trace_metadata_key::ltn);
        trace_metadata_keys.erase(exseis::Trace_metadata_key::gtn);
        trace_metadata_keys.erase(exseis::Trace_metadata_key::dsdr);

        for (auto key : trace_metadata_keys) {
            CAPTURE(key, to_string(key));
            REQUIRE(
                exseis::file::detail::make_blob_parser_segy(key) != nullptr);
        }
    }

    SECTION ("Comparison with test::segy::Trace") {
        int32_t trace_index = 0;
        auto trace          = exseis::test::segy::Trace{
            trace_index, 0, exseis::segy::Segy_number_format::IEEE_fp32};

        const auto trace_native = trace.native();
        auto trace_binary       = trace.binary();

        {
            INFO("exseis::Trace_metadata_key::raw");
            auto raw_parser = exseis::file::detail::make_blob_parser_segy(
                exseis::Trace_metadata_key::raw);

            auto parsed_type = raw_parser->parsed_type();
            REQUIRE(parsed_type.type == exseis::Type::UInt8);
            REQUIRE(
                parsed_type.count == exseis::segy::segy_trace_header_size());

            std::vector<unsigned char> raw_blob(parsed_type.count);
            REQUIRE(raw_blob.size() == trace_binary.size());

            auto data_read_locations  = raw_parser->data_read_locations();
            auto data_write_locations = raw_parser->data_write_locations();

            {
                INFO("Binary_parser::read");

                std::fill(raw_blob.begin(), raw_blob.end(), 0);

                // Set data locations to read from in trace_binary
                for (auto& loc : data_read_locations) {
                    loc.data = trace_binary.data() + loc.begin;
                }

                raw_parser->read(
                    data_read_locations.begin(), data_read_locations.end(),
                    raw_blob.data());

                REQUIRE(std::equal(
                    raw_blob.cbegin(), raw_blob.cend(), trace_binary.cbegin(),
                    trace_binary.cend()));
            }

            {
                INFO("Binary_parser::write");

                std::fill(raw_blob.begin(), raw_blob.end(), 0);

                // Set data locations to write to in raw_blob
                for (auto& loc : data_write_locations) {
                    loc.data = raw_blob.data() + loc.begin;
                }

                raw_parser->write(
                    data_write_locations.begin(), data_write_locations.end(),
                    trace_binary.data());

                REQUIRE(std::equal(
                    raw_blob.cbegin(), raw_blob.cend(), trace_binary.cbegin(),
                    trace_binary.cend()));
            }
        }

        const auto check = [&](exseis::Trace_metadata_key key,
                               auto test_value) {
            CAPTURE(key, to_string(key), test_value);

            using Type = typename std::decay<decltype(test_value)>::type;

            auto blob_parser = exseis::file::detail::make_blob_parser_segy(key);

            auto data_locations = blob_parser->data_read_locations();

            // Set all data locations to trace_binary for read
            for (auto& data_location : data_locations) {
                data_location.data = trace_binary.data() + data_location.begin;
            }


            REQUIRE(exseis::type<Type> == blob_parser->parsed_type().type);

            // Make a 0-initialized copy of test_value
            Type value;

            blob_parser->read(
                data_locations.begin(), data_locations.end(), &value);

            CAPTURE(value, test_value);

            // Check value and test_value are bit-wise equal after read
            REQUIRE(
                exseis::to_big_endian(value)
                == exseis::to_big_endian(test_value));
        };

        check(
            exseis::Trace_metadata_key::line_trace_index,
            trace_native.line_trace_index);

        check(
            exseis::Trace_metadata_key::file_trace_index,
            trace_native.file_trace_index);

        // check(exseis::Trace_metadata_key::ofr_number,
        // trace_native.ofr_number);
        check(
            exseis::Trace_metadata_key::ofr_trace_index,
            trace_native.ofr_trace_index);

        check(
            exseis::Trace_metadata_key::coordinate_scalar,
            trace_native.coordinate_scalar);
        check(exseis::Trace_metadata_key::source_x, trace_native.src_x);
        check(exseis::Trace_metadata_key::source_y, trace_native.src_y);
        check(exseis::Trace_metadata_key::receiver_x, trace_native.rcv_x);
        check(exseis::Trace_metadata_key::receiver_y, trace_native.rcv_y);

        check(
            exseis::Trace_metadata_key::number_of_samples,
            trace_native.number_of_samples);
        // check(
        //    exseis::Trace_metadata_key::sample_interval,
        //    trace_native.sample_interval);

        check(exseis::Trace_metadata_key::cdp_x, trace_native.cmp_x);
        check(exseis::Trace_metadata_key::cdp_y, trace_native.cmp_y);

        check(exseis::Trace_metadata_key::il, trace_native.in_line);
        check(exseis::Trace_metadata_key::xl, trace_native.cross_line);
    }
}
