#include "exseisdat/piol/file/Input_file_segy.hh"
#include "exseisdat/piol/file/Output_file_segy.hh"

#include "exseisdat/piol/io_driver/IO_driver_distributed_vector.hh"
#include "exseisdat/utils/distributed_vector/Distributed_vector_mpi.hh"

#include "exseisdat/piol/configuration/ExSeis.hh"
#include "exseisdat/piol/metadata/Trace_metadata_key.hh"
#include "exseisdat/piol/segy/Trace_header_offsets.hh"
#include "exseisdat/utils/encoding/character_encoding.hh"
#include "exseisdat/utils/encoding/number_encoding.hh"

#include "exseisdat/test/catch2.hh"
#include "exseisdat/test/segy/Segy_file_generator.hh"

#include <algorithm>
#include <iterator>
#include <memory>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

using namespace exseis::piol;
using namespace exseis::utils;


namespace {

class Inspector_segy_file {
    exseis::test::segy::Segy_file m_segy_file;
    std::shared_ptr<exseis::piol::ExSeis> m_piol;

  public:
    Inspector_segy_file(
        exseis::test::segy::Text_header::Encoding text_encoding,
        segy::Segy_number_format number_encoding,
        int32_t number_of_traces,
        int16_t samples_per_trace) :
        m_segy_file{text_encoding, number_encoding, number_of_traces,
                    samples_per_trace},
        m_piol{exseis::piol::ExSeis::make()}
    {
    }

    const exseis::test::segy::Segy_file& segy_file() const
    {
        return m_segy_file;
    }
    std::shared_ptr<exseis::piol::ExSeis> piol() const { return m_piol; }

    exseis::piol::IO_driver io_driver() const
    {
        auto distributed_vector = Distributed_vector_mpi<unsigned char>{
            m_segy_file.size(), MPI_COMM_WORLD};

        return exseis::piol::IO_driver_distributed_vector{
            std::move(distributed_vector)};
    }
};

}  // namespace


TEST_CASE("Output_file_segy", "[Output_file_segy][Output_file][PIOL]")
{
    int32_t number_of_traces  = GENERATE(0, 1, 128);
    int16_t samples_per_trace = GENERATE(0, 1, 128);


    // We only care about writing ASCII and IEEE_fp32 data.
    auto text_encoding   = exseis::test::segy::Text_header::Encoding::ascii;
    auto number_encoding = segy::Segy_number_format::IEEE_fp32;

    auto inspector        = Inspector_segy_file{text_encoding, number_encoding,
                                         number_of_traces, samples_per_trace};
    auto segy_file        = inspector.segy_file();
    auto segy_file_native = segy_file.native();

    auto piol = inspector.piol();

    exseis::piol::Output_file_segy output_file_segy{
        inspector.io_driver(), piol, "Output_file_segy::Distributed_vector"};


    //
    // Write file metadata
    //

    const auto& text    = segy_file_native.text_header.ascii;
    const auto text_end = std::find(text.cbegin(), text.cend(), '\0');
    output_file_segy.write_text(std::string{text.cbegin(), text_end});

    output_file_segy.write_samples_per_trace(
        segy_file_native.binary_header.samples_per_trace);

    output_file_segy.write_number_of_traces(segy_file_native.traces.size());

    output_file_segy.write_sample_interval(
        segy_file_native.binary_header.sample_interval
        * Output_file_segy::Options{}.sample_interval_factor);

    output_file_segy.sync();


    SECTION ("File Metadata") {

        //
        // Read file metadata
        //

        auto io_drivers = std::move(output_file_segy).io_drivers();
        REQUIRE(io_drivers.size() == 1);

        exseis::piol::Input_file_segy input_file_segy{
            std::move(io_drivers[0]), inspector.piol(),
            "Output_file_segy::io_driver()"};

        {
            INFO("Output_file_segy::write_text()");

            const auto input_file_text = input_file_segy.read_text();
            const auto segy_file_text  = segy_file_native.text_header.ascii;
            for (size_t i = 0; i < segy_file_native.text_header.ascii.size();
                 i++) {
                CAPTURE(
                    i, uint32_t(input_file_text[i]),
                    uint32_t(segy_file_text[i]));
                REQUIRE(input_file_text[i] == segy_file_text[i]);
            }
        }

        {
            INFO("Output_file_segy::write_samples_per_trace()");

            const auto input_file_samples_per_trace =
                input_file_segy.read_samples_per_trace();
            const auto segy_file_samples_per_trace =
                segy_file_native.binary_header.samples_per_trace;

            REQUIRE(segy_file_samples_per_trace >= 0);
            REQUIRE(
                input_file_samples_per_trace
                == size_t(segy_file_samples_per_trace));
        }

        {
            INFO("Output_file_segy::write_number_of_traces()");

            const auto input_file_number_of_traces =
                input_file_segy.read_number_of_traces();
            const auto segy_file_number_of_traces =
                segy_file_native.traces.size();

            REQUIRE(input_file_number_of_traces == segy_file_number_of_traces);
        }

        {
            INFO("Output_file_segy::write_sample_interval()");

            const auto input_file_sample_interval =
                input_file_segy.read_sample_interval();
            const auto segy_file_sample_interval =
                segy_file_native.binary_header.sample_interval;

            // TODO: Remove sample_interval_factor from Input_file_segy ... ?
            REQUIRE(
                input_file_sample_interval
                == segy_file_sample_interval
                       * Output_file_segy::Options{}.sample_interval_factor);
        }
    }


    SECTION ("File Traces") {

        //
        // Set / Test Trace metadata
        //
        const auto for_each_metadata = [&](size_t trace_index, auto& integer_op,
                                           auto& floating_op) {
            using Key = exseis::piol::Trace_metadata_key;

            const auto& trace_native = segy_file_native.traces[trace_index];

            integer_op(Key::tnl, trace_native.line_trace_index);
            integer_op(Key::tn, trace_native.file_trace_index);
            integer_op(Key::tnr, trace_native.ofr_trace_index);

            floating_op(Key::x_src, trace_native.src_x);
            floating_op(Key::y_src, trace_native.src_y);
            floating_op(Key::x_rcv, trace_native.rcv_x);
            floating_op(Key::y_rcv, trace_native.rcv_y);

            integer_op(Key::ns, trace_native.number_of_samples);

            floating_op(Key::xCmp, trace_native.cmp_x);
            floating_op(Key::yCmp, trace_native.cmp_y);
            integer_op(Key::il, trace_native.in_line);
            integer_op(Key::xl, trace_native.cross_line);
        };

        const auto set_trace_metadata = [&](size_t trace_index,
                                            size_t trace_buffer_index,
                                            Trace_metadata& trace_metadata) {
            const auto set_integer = [&](auto key, auto value) {
                trace_metadata.set_integer(trace_buffer_index, key, value);
            };

            const auto set_floating = [&](auto key, auto value) {
                trace_metadata.set_floating_point(
                    trace_buffer_index, key, value);
            };

            for_each_metadata(trace_index, set_integer, set_floating);
        };

        const auto check_trace_metadata =
            [&](size_t trace_index, size_t trace_buffer_index,
                const Trace_metadata& trace_metadata) {
                const auto check_integer = [&](auto key, auto value) {
                    CAPTURE(key);
                    REQUIRE(
                        trace_metadata.get_integer(trace_buffer_index, key)
                        == value);
                };

                const auto check_floating = [&](auto key, auto value) {
                    CAPTURE(key);
                    REQUIRE(
                        trace_metadata.get_floating_point(
                            trace_buffer_index, key)
                        == value);
                };

                for_each_metadata(trace_index, check_integer, check_floating);
            };

        const auto check_trace_metadata_is_zero =
            [&](size_t trace_index, size_t trace_buffer_index,
                const Trace_metadata& trace_metadata) {
                const auto check_integer = [&](auto key, auto /*value*/) {
                    CAPTURE(key);
                    REQUIRE(
                        trace_metadata.get_integer(trace_buffer_index, key)
                        == 0);
                };

                const auto check_floating = [&](auto key, auto /*value*/) {
                    CAPTURE(key);
                    REQUIRE(
                        trace_metadata.get_floating_point(
                            trace_buffer_index, key)
                        == Approx(0));
                };

                for_each_metadata(trace_index, check_integer, check_floating);
            };


        //
        // Set / Test Trace Data
        //
        const auto set_trace_data =
            [&](size_t trace_index, size_t trace_buffer_index,
                std::vector<exseis::utils::Trace_value>& trace_data) {
                const auto& samples =
                    segy_file_native.traces[trace_index].samples;

                const auto trace_begin = std::next(
                    trace_data.begin(), trace_buffer_index * samples_per_trace);
                CAPTURE(trace_buffer_index);
                assert(samples.empty() || trace_begin < trace_data.end());

                std::copy(
                    std::cbegin(samples), std::cend(samples), trace_begin);
            };

        const auto check_trace_data =
            [&](size_t trace_index, size_t trace_buffer_index,
                const std::vector<exseis::utils::Trace_value>& trace_data) {
                // Get iterators for trace_data
                const auto trace_begin = std::next(
                    trace_data.cbegin(),
                    trace_buffer_index * samples_per_trace);
                const auto trace_end =
                    std::next(trace_begin, samples_per_trace);

                const auto& samples_i =
                    segy_file_native.traces[trace_index].samples;

                REQUIRE(
                    size_t(std::distance(trace_begin, trace_end))
                    == samples_i.size());

                REQUIRE(std::equal(
                    trace_begin, trace_end, samples_i.cbegin(),
                    samples_i.cend(), [](auto t, auto s) {
                        // bit-wise comparison of floats (!)
                        return exseis::utils::to_big_endian(t)
                               == exseis::utils::to_big_endian(s);
                    }));
            };

        const auto check_trace_data_is_zero =
            [&](size_t /*trace_index*/, size_t trace_buffer_index,
                const std::vector<Trace_value>& trace_data) {
                const auto trace_begin = std::next(
                    trace_data.cbegin(),
                    trace_buffer_index * samples_per_trace);
                const auto trace_end =
                    std::next(trace_begin, samples_per_trace);

                REQUIRE(std::all_of(trace_begin, trace_end, [](auto v) {
                    return !(v < 0) && !(v > 0);
                }));
            };

        SECTION ("Output_file_segy::write_*()") {
            const size_t max_chunk_size_pct = GENERATE(0, 10, 90, 100);

            const size_t max_trace_index_chunk_size = std::max<size_t>(
                (number_of_traces * max_chunk_size_pct) / 100, 1);
            const size_t skip = 0;

            enum class Method { metadata_only, data_only, metadata_and_data };
            const auto method = GENERATE(
                Method::metadata_only, Method::data_only,
                Method::metadata_and_data);

            const auto for_each_chunk = [&](auto f) {
                for (size_t trace_index_begin = 0;
                     trace_index_begin < size_t(number_of_traces);
                     trace_index_begin += max_trace_index_chunk_size) {
                    const auto trace_index_end = std::min<size_t>(
                        trace_index_begin + max_trace_index_chunk_size,
                        number_of_traces);
                    f(trace_index_begin, trace_index_end);
                }
            };

            const auto for_each_in_chunk = [&](size_t trace_index_begin,
                                               size_t trace_index_end, auto f,
                                               auto&&... args) {
                for (size_t trace_index = trace_index_begin;
                     trace_index != trace_index_end; trace_index++) {
                    const size_t trace_buffer_index =
                        trace_index - trace_index_begin;
                    f(trace_index, trace_buffer_index, args...);
                }
            };

            for_each_chunk([&](size_t trace_index_begin,
                               size_t trace_index_end) {
                const auto trace_index_chunk_size =
                    trace_index_end - trace_index_begin;

                std::vector<Trace_value> trace_data;
                trace_data.resize(samples_per_trace * trace_index_chunk_size);

                Trace_metadata trace_metadata{Rule{true, true, true},
                                              trace_index_chunk_size};

                const auto set_chunk_metadata = [&] {
                    for_each_in_chunk(
                        trace_index_begin, trace_index_end, set_trace_metadata,
                        trace_metadata);
                };
                const auto set_chunk_data = [&] {
                    for_each_in_chunk(
                        trace_index_begin, trace_index_end, set_trace_data,
                        trace_data);
                };

                switch (method) {
                    case Method::metadata_only:
                        if (piol->get_rank() == 0) {
                            set_chunk_metadata();

                            output_file_segy.write_metadata(
                                trace_index_begin, trace_index_chunk_size,
                                trace_metadata, skip);
                        }
                        else {
                            output_file_segy.write_metadata();
                        }
                        break;

                    case Method::data_only:
                        if (piol->get_rank() == 0) {
                            set_chunk_data();

                            output_file_segy.write_data(
                                trace_index_begin, trace_index_chunk_size,
                                trace_data.data());
                        }
                        else {
                            output_file_segy.write_data();
                        }
                        break;

                    case Method::metadata_and_data:
                        if (piol->get_rank() == 0) {
                            set_chunk_metadata();
                            set_chunk_data();

                            output_file_segy.write(
                                trace_index_begin, trace_index_chunk_size,
                                trace_data.data(), trace_metadata, skip);
                        }
                        else {
                            output_file_segy.write();
                        }
                        break;
                }
            });

            output_file_segy.sync();
            auto io_drivers = std::move(output_file_segy).io_drivers();
            REQUIRE(io_drivers.size() == 1);

            Input_file_segy input_file_segy{std::move(io_drivers[0]),
                                            inspector.piol(),
                                            "Output_file_segy::io_drivers()"};

            for_each_chunk([&](size_t trace_index_begin,
                               size_t trace_index_end) {
                const auto trace_index_chunk_size =
                    trace_index_end - trace_index_begin;

                std::vector<Trace_value> trace_data;
                trace_data.resize(samples_per_trace * trace_index_chunk_size);

                Trace_metadata trace_metadata{Rule{true, true, true},
                                              trace_index_chunk_size};

                input_file_segy.read(
                    trace_index_begin, trace_index_chunk_size,
                    trace_data.data(), trace_metadata, skip);

                const auto check_chunk_metadata = [&] {
                    for_each_in_chunk(
                        trace_index_begin, trace_index_end,
                        check_trace_metadata, trace_metadata);
                };
                const auto check_chunk_metadata_is_zero = [&] {
                    for_each_in_chunk(
                        trace_index_begin, trace_index_end,
                        check_trace_metadata_is_zero, trace_metadata);
                };

                const auto check_chunk_data = [&] {
                    for_each_in_chunk(
                        trace_index_begin, trace_index_end, check_trace_data,
                        trace_data);
                };
                const auto check_chunk_data_is_zero = [&] {
                    for_each_in_chunk(
                        trace_index_begin, trace_index_end,
                        check_trace_data_is_zero, trace_data);
                };

                switch (method) {
                    case Method::metadata_only:
                        check_chunk_metadata();
                        check_chunk_data_is_zero();
                        break;

                    case Method::data_only:
                        check_chunk_metadata_is_zero();
                        check_chunk_data();
                        break;

                    case Method::metadata_and_data:
                        check_chunk_metadata();
                        check_chunk_data();
                        break;
                }
            });
        }

        SECTION ("Output_file_segy::write_*_non_contiguous()") {
            const size_t max_chunk_size_pct = GENERATE(0, 10, 90, 100);

            enum class Method { metadata_only, data_only, metadata_and_data };
            const auto method = GENERATE(
                Method::metadata_only, Method::data_only,
                Method::metadata_and_data);

            const size_t max_offset_index_chunk_size = std::max<size_t>(
                (number_of_traces * max_chunk_size_pct) / 100, 1);
            const size_t skip = 0;

            std::vector<size_t> offsets = ([&] {
                std::vector<size_t> offsets(number_of_traces);
                std::iota(offsets.begin(), offsets.end(), size_t(0));

                std::mt19937_64 mt{0};
                std::shuffle(offsets.begin(), offsets.end(), mt);

                std::iota(offsets.begin(), offsets.end(), 0);

                return offsets;
            }());

            const auto for_each_chunk = [&](auto f, auto&&... args) {
                for (size_t offset_index_begin = 0;
                     offset_index_begin < size_t(number_of_traces);
                     offset_index_begin += max_offset_index_chunk_size) {

                    const auto offset_index_end = std::min<size_t>(
                        offset_index_begin + max_offset_index_chunk_size,
                        number_of_traces);

                    f(offset_index_begin, offset_index_end, args...);
                }
            };

            // NOTE: Iterates over trace indices in the chunk,
            //       NOT offset indices!
            const auto for_each_in_chunk = [&](size_t offset_index_begin,
                                               size_t offset_index_end, auto f,
                                               auto&&... args) {
                for (size_t offset_index = offset_index_begin;
                     offset_index < offset_index_end; offset_index++) {

                    const size_t trace_index = offsets[offset_index];
                    const size_t trace_buffer_index =
                        offset_index - offset_index_begin;

                    f(trace_index, trace_buffer_index, args...);
                }
            };

            for_each_chunk([&](size_t offset_index_begin,
                               size_t offset_index_end) {
                const auto offset_index_chunk_size =
                    offset_index_end - offset_index_begin;

                std::vector<Trace_value> trace_data;
                trace_data.resize(samples_per_trace * offset_index_chunk_size);

                Trace_metadata trace_metadata{Rule{true, true, true},
                                              offset_index_chunk_size};


                for_each_in_chunk(
                    offset_index_begin, offset_index_end, set_trace_metadata,
                    trace_metadata);
                for_each_in_chunk(
                    offset_index_begin, offset_index_end, set_trace_data,
                    trace_data);

                switch (method) {
                    case Method::metadata_only:
                        if (piol->get_rank() == 0) {
                            output_file_segy.write_metadata_non_contiguous(
                                offset_index_chunk_size,
                                offsets.data() + offset_index_begin,
                                trace_metadata, skip);
                        }
                        else {
                            output_file_segy.write_metadata_non_contiguous();
                        }
                        break;

                    case Method::data_only:
                        if (piol->get_rank() == 0) {
                            output_file_segy.write_data_non_contiguous(
                                offset_index_chunk_size,
                                offsets.data() + offset_index_begin,
                                trace_data.data());
                        }
                        else {
                            output_file_segy.write_data_non_contiguous();
                        }
                        break;

                    case Method::metadata_and_data:
                        if (piol->get_rank() == 0) {
                            output_file_segy.write_non_contiguous(
                                offset_index_chunk_size,
                                offsets.data() + offset_index_begin,
                                trace_data.data(), trace_metadata, skip);
                        }
                        else {
                            output_file_segy.write_non_contiguous();
                        }
                        break;
                }
            });

            output_file_segy.sync();
            auto io_drivers = std::move(output_file_segy).io_drivers();
            REQUIRE(io_drivers.size() == 1);

            const Input_file_segy input_file_segy{
                std::move(io_drivers[0]), inspector.piol(),
                "Output_file_segy::io_drivers()"};

            for_each_chunk([&](size_t offset_index_begin,
                               size_t offset_index_end) {
                const auto offset_index_chunk_size =
                    offset_index_end - offset_index_begin;

                std::vector<Trace_value> trace_data;
                trace_data.resize(samples_per_trace * offset_index_chunk_size);

                Trace_metadata trace_metadata{Rule{true, true, true},
                                              offset_index_chunk_size};

                input_file_segy.read_non_contiguous(
                    offset_index_chunk_size,
                    offsets.data() + offset_index_begin, trace_data.data(),
                    trace_metadata, skip);

                const auto for_each_in_this_chunk = [&](auto&&... args) {
                    for_each_in_chunk(
                        offset_index_begin, offset_index_end, args...);
                };
                const auto check_chunk_metadata = [&] {
                    for_each_in_this_chunk(
                        check_trace_metadata, trace_metadata);
                };
                const auto check_chunk_metadata_is_zero = [&] {
                    for_each_in_this_chunk(
                        check_trace_metadata_is_zero, trace_metadata);
                };
                const auto check_chunk_data = [&] {
                    for_each_in_this_chunk(check_trace_data, trace_data);
                };
                const auto check_chunk_data_is_zero = [&] {
                    for_each_in_this_chunk(
                        check_trace_data_is_zero, trace_data);
                };

                switch (method) {
                    case Method::metadata_only:
                        check_chunk_metadata();
                        check_chunk_data_is_zero();
                        break;

                    case Method::data_only:
                        check_chunk_metadata_is_zero();
                        check_chunk_data();
                        break;

                    case Method::metadata_and_data:
                        check_chunk_metadata();
                        check_chunk_data();
                        break;
                }
            });
        }
    }
}
