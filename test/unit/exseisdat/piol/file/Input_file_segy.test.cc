#include "exseisdat/piol/file/Input_file_segy.hh"

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

        auto io_driver = exseis::piol::IO_driver_distributed_vector{
            std::move(distributed_vector)};

        if (m_piol->get_rank() == 0) {
            io_driver.write(0, m_segy_file.size(), m_segy_file.binary().data());
        }
        else {
            io_driver.write(0, 0, nullptr);
        }

        io_driver.sync();

        return std::move(io_driver);
    }
};

}  // namespace

TEST_CASE("Input_file_segy", "[Input_file_segy][Input_file][PIOL]")
{
    int32_t number_of_traces  = GENERATE(0, 1, 128);
    int16_t samples_per_trace = GENERATE(0, 1, 128);


    auto text_encoding = GENERATE(
        exseis::test::segy::Text_header::Encoding::ascii,
        exseis::test::segy::Text_header::Encoding::ebcdic);
    auto number_encoding = GENERATE(
        segy::Segy_number_format::IBM_fp32,
        segy::Segy_number_format::IEEE_fp32);

    auto inspector        = Inspector_segy_file{text_encoding, number_encoding,
                                         number_of_traces, samples_per_trace};
    auto segy_file        = inspector.segy_file();
    auto segy_file_native = segy_file.native();

    const exseis::piol::Input_file_segy input_file_segy{
        inspector.io_driver(), inspector.piol(),
        "Input_file_segy::Distributed_vector"};

    SECTION ("File Metadata") {
        {
            INFO("Input_file_segy::read_text()");

            const auto& input_file_text = input_file_segy.read_text();
            const auto& segy_file_text  = segy_file_native.text_header.ascii;
            for (size_t i = 0; i < segy_file_native.text_header.ascii.size();
                 i++) {
                CAPTURE(
                    i, uint32_t(input_file_text[i]),
                    uint32_t(segy_file_text[i]));
                REQUIRE(input_file_text[i] == segy_file_text[i]);
            }
        }

        {
            INFO("Input_file_segy::read_ns()");

            const auto& input_file_ns = input_file_segy.read_ns();
            const auto& segy_file_ns =
                segy_file_native.binary_header.samples_per_trace;

            REQUIRE(segy_file_ns >= 0);
            REQUIRE(input_file_ns == size_t(segy_file_ns));
        }

        {
            INFO("Input_file_segy::read_nt()");

            const auto& input_file_nt = input_file_segy.read_nt();
            const auto& segy_file_nt  = segy_file_native.traces.size();

            REQUIRE(input_file_nt == segy_file_nt);
        }

        {
            INFO("Input_file_segy::read_sample_interval()");

            const auto& input_file_sample_interval =
                input_file_segy.read_sample_interval();
            const auto& segy_file_sample_interval =
                segy_file_native.binary_header.sample_interval;

            // TODO: Remove sample_interval_factor from Input_file_segy ... ?
            REQUIRE(
                input_file_sample_interval
                == segy_file_sample_interval
                       * Input_file_segy::Options{}.sample_interval_factor);
        }
    }


    // Test Trace metadata
    const auto check_trace_metadata =
        [&](size_t trace_index, size_t trace_buffer_index,
            const Trace_metadata& trace_metadata) {
            using Key = exseis::piol::Trace_metadata_key;

            const auto check_integer = [&](Key key, auto value) {
                CAPTURE(key);
                REQUIRE(
                    trace_metadata.get_integer(trace_buffer_index, key)
                    == value);
            };

            const auto check_floating = [&](Key key, auto value) {
                CAPTURE(key);
                REQUIRE(
                    trace_metadata.get_floating_point(trace_buffer_index, key)
                    == value);
            };

            const auto& trace_native = segy_file_native.traces[trace_index];

            check_integer(Key::tnl, trace_native.line_trace_index);
            check_integer(Key::tn, trace_native.file_trace_index);
            check_integer(Key::tnr, trace_native.ofr_trace_index);

            check_floating(Key::x_src, trace_native.src_x);
            check_floating(Key::y_src, trace_native.src_y);
            check_floating(Key::x_rcv, trace_native.rcv_x);
            check_floating(Key::y_rcv, trace_native.rcv_y);

            check_integer(Key::ns, trace_native.number_of_samples);

            check_floating(Key::xCmp, trace_native.cmp_x);
            check_floating(Key::yCmp, trace_native.cmp_y);
            check_integer(Key::il, trace_native.in_line);
            check_integer(Key::xl, trace_native.cross_line);
        };

    // Test trace data
    const auto check_trace_data =
        [&](size_t trace_index, size_t trace_buffer_index,
            const std::vector<exseis::utils::Trace_value>& trace_data) {
            // Get iterators for trace_data
            const auto trace_begin = std::next(
                trace_data.cbegin(), trace_buffer_index * samples_per_trace);
            const auto trace_end = std::next(trace_begin, samples_per_trace);

            const auto& samples_i =
                segy_file_native.traces[trace_index].samples;

            REQUIRE(
                size_t(std::distance(trace_begin, trace_end))
                == samples_i.size());

            REQUIRE(std::equal(
                trace_begin, trace_end, samples_i.cbegin(), samples_i.cend(),
                [](auto t, auto s) {
                    // bit-wise comparison of floats (!)
                    return exseis::utils::to_big_endian(t)
                           == exseis::utils::to_big_endian(s);
                }));
        };

    SECTION ("Input_file_segy::read_*()") {

        const size_t number_of_traces = segy_file_native.traces.size();
        const size_t samples_per_trace =
            segy_file_native.binary_header.samples_per_trace;
        const size_t skip = 0;


        if (number_of_traces == 0) {
            INFO("number_of_traces == 0");

            {
                INFO("traces == nullptr");
                input_file_segy.read_trace(
                    0, number_of_traces, nullptr, nullptr, skip);
                SUCCEED();
            }

            {
                INFO("traces != nullptr");

                // Using non-zero sized vector, because std::vector::data() can
                // return nullptr when zero-sized.
                std::vector<exseis::utils::Trace_value> trace_data;
                trace_data.resize(1);

                input_file_segy.read_trace(
                    0, number_of_traces, trace_data.data(), nullptr, skip);

                REQUIRE(
                    exseis::utils::to_big_endian(trace_data[0])
                    == exseis::utils::to_big_endian(
                        exseis::utils::Trace_value(0)));
            }
        }
        else {
            INFO("number_of_traces > 0");

            // Reading different numbers of traces in the one go
            const size_t max_trace_index_chunk_size_percent =
                GENERATE(0u, 10u, 50u, 90u, 100u);

            // Get chunk size from percentage. Chunk size must be at least 1.
            size_t max_trace_index_chunk_size = std::max<size_t>(
                max_trace_index_chunk_size_percent * number_of_traces / 100, 1);

            // Iterate over chunks
            for (size_t trace_index_begin = 0;
                 trace_index_begin < number_of_traces;
                 trace_index_begin += max_trace_index_chunk_size) {

                const auto trace_index_end = std::min<size_t>(
                    trace_index_begin + max_trace_index_chunk_size,
                    number_of_traces);
                const auto trace_index_chunk_size =
                    trace_index_end - trace_index_begin;


                // Iterate over indices in the chunk.
                const auto for_each_in_chunk = [&](auto f, auto&&... args) {
                    for (size_t trace_index = trace_index_begin;
                         trace_index != trace_index_end; trace_index++) {

                        const size_t trace_buffer_index =
                            trace_index - trace_index_begin;

                        f(trace_index, trace_buffer_index, args...);
                    }
                };

                {
                    INFO(
                        "Input_file_segy::read_trace(Trace_data*, Trace_metadata*)");

                    std::vector<exseis::utils::Trace_value> trace_data;
                    trace_data.resize(
                        samples_per_trace * trace_index_chunk_size);

                    exseis::piol::Trace_metadata trace_metadata(
                        exseis::piol::Rule{true, true, true},
                        trace_index_chunk_size);

                    input_file_segy.read_trace(
                        trace_index_begin, trace_index_chunk_size,
                        trace_data.data(), &trace_metadata, skip);

                    for_each_in_chunk(check_trace_metadata, trace_metadata);
                    for_each_in_chunk(check_trace_data, trace_data);
                }

                {
                    INFO("Input_file_segy::read_trace(Trace_data*)");

                    std::vector<exseis::utils::Trace_value> trace_data;
                    trace_data.resize(
                        samples_per_trace * trace_index_chunk_size);

                    input_file_segy.read_trace(
                        trace_index_begin, trace_index_chunk_size,
                        trace_data.data());

                    for_each_in_chunk(check_trace_data, trace_data);
                }

                {
                    INFO(
                        "Input_file_segy::read_trace(nullptr, Trace_metadata*)");

                    exseis::piol::Trace_metadata trace_metadata(
                        exseis::piol::Rule{true, true, true},
                        trace_index_chunk_size);

                    input_file_segy.read_trace(
                        trace_index_begin, trace_index_chunk_size, nullptr,
                        &trace_metadata, skip);

                    for_each_in_chunk(check_trace_metadata, trace_metadata);
                }

                {
                    INFO("Input_file_segy::read_param()");

                    exseis::piol::Trace_metadata trace_metadata(
                        exseis::piol::Rule{true, true, true},
                        trace_index_chunk_size);

                    input_file_segy.read_param(
                        trace_index_begin, trace_index_chunk_size,
                        &trace_metadata, skip);

                    for_each_in_chunk(check_trace_metadata, trace_metadata);
                }
            }
        }
    }

    SECTION ("Input_file_segy::read_*_non_contiguous()") {

        const size_t number_of_traces = segy_file_native.traces.size();
        const size_t samples_per_trace =
            segy_file_native.binary_header.samples_per_trace;
        const size_t skip = 0;

        if (number_of_traces == 0) {
            INFO("number_of_traces == 0");

            {
                INFO("traces == nullptr");
                input_file_segy.read_trace_non_contiguous(
                    number_of_traces, nullptr, nullptr, nullptr, skip);
                SUCCEED();
            }

            {
                INFO("traces != nullptr");

                // Using non-zero sized vector, because std::vector::data() can
                // return nullptr when zero-sized.
                std::vector<exseis::utils::Trace_value> trace_data;
                trace_data.resize(1);

                input_file_segy.read_trace_non_contiguous(
                    number_of_traces, nullptr, trace_data.data(), nullptr,
                    skip);

                REQUIRE(
                    exseis::utils::to_big_endian(trace_data[0])
                    == exseis::utils::to_big_endian(
                        exseis::utils::Trace_value(0)));
            }
        }
        else {
            INFO("number_of_traces > 0");

            // Reading different numbers of traces in the one go.
            // Read minimum 1 trace at a time
            const size_t max_chunk_size_percent =
                GENERATE(0u, 10u, 50u, 90u, 100u);
            const size_t max_chunk_size = std::max<size_t>(
                (number_of_traces * max_chunk_size_percent) / 100, 1);

            // Generate sorted list of unique indices
            std::vector<size_t> offsets = ([=] {
                std::vector<size_t> indices(number_of_traces);
                std::iota(indices.begin(), indices.end(), size_t(0));

                std::mt19937_64 mt{0};
                std::shuffle(indices.begin(), indices.end(), mt);

                return indices;
            }());
            for (const auto& offset : offsets) {
                assert(offset < number_of_traces);
                (void)offset;
            }

            for (size_t chunk_start = 0; chunk_start < number_of_traces;
                 chunk_start += max_chunk_size) {
                const size_t chunk_end = std::min<size_t>(
                    chunk_start + max_chunk_size, number_of_traces);

                const size_t chunk_size = chunk_end - chunk_start;


                const auto for_each_in_chunk = [&](auto f, auto&&... args) {
                    for (size_t offset_index = chunk_start;
                         offset_index < chunk_end; offset_index++) {
                        const auto trace_index = offsets[offset_index];
                        const auto trace_buffer_index =
                            offset_index - chunk_start;

                        f(trace_index, trace_buffer_index, args...);
                    }
                };

                {
                    INFO(
                        "Input_file_segy::read_trace_non_contiguous(Trace_data*, Trace_metadata*)");

                    std::vector<exseis::utils::Trace_value> trace_data;
                    trace_data.resize(samples_per_trace * chunk_size);

                    exseis::piol::Trace_metadata trace_metadata(
                        exseis::piol::Rule{true, true, true}, chunk_size);

                    input_file_segy.read_trace_non_contiguous(
                        chunk_size, offsets.data() + chunk_start,
                        trace_data.data(), &trace_metadata, skip);

                    for_each_in_chunk(check_trace_metadata, trace_metadata);
                    for_each_in_chunk(check_trace_data, trace_data);
                }

                {
                    INFO(
                        "Input_file_segy::read_trace_non_contiguous(Trace_data*)");

                    std::vector<exseis::utils::Trace_value> trace_data;
                    trace_data.resize(samples_per_trace * chunk_size);

                    std::fill(trace_data.begin(), trace_data.end(), 0);
                    input_file_segy.read_trace_non_contiguous(
                        chunk_size, offsets.data() + chunk_start,
                        trace_data.data());

                    for_each_in_chunk(check_trace_data, trace_data);
                }

                {
                    INFO(
                        "Input_file_segy::read_trace_non_contiguous(nullptr, Trace_metadata*)");

                    exseis::piol::Trace_metadata trace_metadata(
                        exseis::piol::Rule{true, true, true}, chunk_size);

                    input_file_segy.read_trace_non_contiguous(
                        chunk_size, offsets.data() + chunk_start, nullptr,
                        &trace_metadata, skip);

                    for_each_in_chunk(check_trace_metadata, trace_metadata);
                }

                {
                    INFO("Input_file_segy::read_param_non_contiguous()");

                    exseis::piol::Trace_metadata trace_metadata(
                        exseis::piol::Rule{true, true, true}, chunk_size);

                    input_file_segy.read_param_non_contiguous(
                        chunk_size, offsets.data() + chunk_start,
                        &trace_metadata, skip);

                    for_each_in_chunk(check_trace_metadata, trace_metadata);
                }
            }
        }
    }
}
