#include "exseisdat/test/segy/Segy_file_generator.hh"

#include "exseisdat/test/catch2.hh"


TEST_CASE("Text_header", "[test][segy][Segy_file_generator][Text_header]")
{
    using Text_header = exseis::test::segy::Text_header;

    auto encoding =
        GENERATE(Text_header::Encoding::ascii, Text_header::Encoding::ebcdic);

    Text_header text_header(encoding);

    REQUIRE(text_header.size() == exseis::piol::segy::segy_text_header_size());

    SECTION ("Text_header::Native") {
        auto th_native = text_header.native();

        REQUIRE(
            th_native.ascii.size()
            == exseis::piol::segy::segy_text_header_size());
        REQUIRE(([&] {
            for (size_t i = 0; i < th_native.ascii.size(); i++) {
                if (th_native.ascii[i] != Text_header::ascii_pattern(i)) {
                    return false;
                }
            }
            return true;
        }()));
        REQUIRE(th_native.encoding == encoding);
    }

    SECTION ("Text_header::Binary") {
        auto th_binary = text_header.binary();

        REQUIRE(
            th_binary.size() == exseis::piol::segy::segy_text_header_size());
        REQUIRE(([&] {
            for (size_t i = 0; i < th_binary.size(); i++) {
                switch (encoding) {
                    case Text_header::Encoding::ascii:
                        if (th_binary[i] != Text_header::ascii_pattern(i)) {
                            return false;
                        }
                        break;

                    case Text_header::Encoding::ebcdic:
                        if (th_binary[i] != Text_header::ebcdic_pattern(i)) {
                            return false;
                        }
                        break;

                    default:
                        assert(false && "Unknown encoding!");
                        return false;
                }
            }
            return true;
        }()));
    }
}


TEST_CASE("Trace", "[test][segy][Segy_file_generator][Trace]")
{
    const int16_t samples_per_trace = GENERATE(0, 1, range(128, 1024, 128));
    const auto number_encoding      = GENERATE(
        exseis::piol::segy::Segy_number_format::IBM_fp32,
        exseis::piol::segy::Segy_number_format::IEEE_fp32);

    const int32_t trace_index = GENERATE(0, 1, 1024);

    const auto trace = exseis::test::segy::Trace{trace_index, samples_per_trace,
                                                 number_encoding};

    REQUIRE(
        trace.native().samples.size()
        == static_cast<size_t>(samples_per_trace));

    // Currently expecting only 4-byte types
    REQUIRE(trace.bytes_per_sample(number_encoding) == 4);

    constexpr size_t trace_header_size = 240;
    REQUIRE(
        trace.size()
        == (trace_header_size
            + samples_per_trace * trace.bytes_per_sample(number_encoding)));


    REQUIRE(trace.size() == trace.binary().size());

    {
        INFO("Trace headers");

        const auto trace_native = trace.native();
        const auto trace_binary = trace.binary();

        // Trace file number
        REQUIRE(
            trace_native.file_trace_index
            == static_cast<int32_t>(trace_index) + 1);
        const unsigned char* file_trace_index_ptr =
            trace_binary.data()
            + (static_cast<size_t>(
                   exseis::piol::segy::Trace_header_offsets::SeqFNum)
               - 1);
        REQUIRE(
            exseis::utils::from_big_endian<int32_t>(
                file_trace_index_ptr[0], file_trace_index_ptr[1],
                file_trace_index_ptr[2], file_trace_index_ptr[3])
            == int32_t(trace_native.file_trace_index));
    }

    {
        INFO("samples_pattern(i).native() <-> samples_pattern(i).binary()");
        for (int16_t i = 0; i < samples_per_trace; i++) {
            const auto ibm_number = trace.samples_pattern(i);

            const auto binary_be = ibm_number.binary();
            const auto native    = ibm_number.native();

            const auto binary_converted =
                exseis::utils::from_ibm_to_float(binary_be, true);

            REQUIRE(
                exseis::utils::to_big_endian(native)
                == exseis::utils::to_big_endian(binary_converted));
        }
    }
    {
        INFO("trace.samples[i] == trace.samples_pattern_ieee(i)");
        const auto trace_native = trace.native();
        for (int16_t i = 0; i < samples_per_trace; i++) {
            const float native_sample        = trace_native.samples[i];
            const float samples_pattern_ieee = trace.samples_pattern_ieee(i);

            REQUIRE(
                exseis::utils::to_big_endian(native_sample)
                == exseis::utils::to_big_endian(samples_pattern_ieee));
        }
    }

    {
        INFO("samples_binary[i] = trace.samples_pattern_*(i)");

        if (number_encoding
            == exseis::piol::segy::Segy_number_format::IBM_fp32) {
            auto trace_native = trace.native();
            auto trace_binary = trace.binary();
            unsigned char* trace_binary_samples =
                trace_binary.data() + trace_header_size;
            for (int16_t i = 0; i < samples_per_trace; i++) {

                unsigned char* sample_begin =
                    trace_binary_samples + i * sizeof(uint32_t);
                std::array<unsigned char, 4> sample_binary_be = {
                    sample_begin[0], sample_begin[1], sample_begin[2],
                    sample_begin[3]};

                REQUIRE(sample_binary_be == trace.samples_pattern_ibm(i));

                const float converted_sample = ([&] {
                    switch (number_encoding) {
                        case exseis::piol::segy::Segy_number_format::IBM_fp32:
                            return exseis::utils::from_ibm_to_float(
                                sample_binary_be, true);
                        case exseis::piol::segy::Segy_number_format::IEEE_fp32:
                            return exseis::utils::from_big_endian<float>(
                                sample_binary_be);
                        case exseis::piol::segy::Segy_number_format::TC4:
                        case exseis::piol::segy::Segy_number_format::TC2:
                        case exseis::piol::segy::Segy_number_format::FPG:
                        case exseis::piol::segy::Segy_number_format::NA1:
                        case exseis::piol::segy::Segy_number_format::NA2:
                        case exseis::piol::segy::Segy_number_format::TC1:
                        default:
                            assert(false && "Unsupported number format");
                    }
                    return 0.0f;
                }());

                const float native_sample = trace_native.samples[i];

                REQUIRE(
                    exseis::utils::to_big_endian(converted_sample)
                    == exseis::utils::to_big_endian(native_sample));
            }
        }
    }
}

TEST_CASE("Segy_file", "[test][segy][Segy_file_generator][Segy_file]")
{
    int32_t number_of_traces  = GENERATE(range(0, 1024, 128));
    int16_t samples_per_trace = GENERATE(range(0, 1024, 128));

    auto text_encoding = GENERATE(
        exseis::test::segy::Text_header::Encoding::ascii,
        exseis::test::segy::Text_header::Encoding::ebcdic);
    auto number_encoding =
        GENERATE(exseis::piol::segy::Segy_number_format::IEEE_fp32);

    const auto segy_file = exseis::test::segy::Segy_file{
        text_encoding, number_encoding, number_of_traces, samples_per_trace};

    constexpr size_t segy_text_header_size   = 3200;
    constexpr size_t segy_binary_header_size = 400;
    constexpr size_t trace_header_size       = 240;
    const size_t sample_size =
        exseis::test::segy::Trace::bytes_per_sample(number_encoding);
    REQUIRE(
        segy_file.size()
        == segy_text_header_size + segy_binary_header_size
               + number_of_traces
                     * (trace_header_size + samples_per_trace * sample_size));

    REQUIRE(segy_file.size() == segy_file.binary().size());
}
