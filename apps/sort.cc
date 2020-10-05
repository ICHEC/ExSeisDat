#include "exseisdat/piol.hh"
#include "exseisdat/utils.hh"

#include <CLI11.hpp>

#include <assert.h>
#include <iostream>

using namespace exseis::utils;
using namespace exseis::piol;

struct Options {
    std::string input_filename;
    std::string output_filename;
    Sort_type sort_type = Sort_type::SrcRcv;
};

Options parse_cli(int argc, char* argv[])
{
    CLI::App app{"Sort SEG-Y File"};

    Options options;

    app.add_option("-i,--input", options.input_filename, "Input file")
        ->required();
    app.add_option("-o,--output", options.output_filename, "Sorted output file")
        ->required();

    size_t sort_type_int = static_cast<size_t>(options.sort_type);
    app.add_option("-t,--type", sort_type_int, "Sort type");

    try {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e) {
        int code = app.exit(e);
        std::exit(code);
    }

    return options;
}

int main(int argc, char** argv)
{
    auto options = parse_cli(argc, argv);

    auto input_filename  = options.input_filename;
    auto output_filename = options.output_filename;
    auto sort_type       = options.sort_type;


    auto piol = ExSeis::make();

    Input_file_segy input_file(piol, input_filename);

    const auto number_of_traces  = input_file.read_nt();
    const auto samples_per_trace = input_file.read_ns();

    // Find number of traces to read on this process
    const auto decomposition = block_decomposition(
        number_of_traces, piol->get_num_rank(), piol->get_rank());

    const auto traces_begin           = decomposition.global_offset;
    const auto local_number_of_traces = decomposition.local_size;

    // Just read everything
    // TODO: only read metadata actually used by sort!
    auto trace_metadata =
        Trace_metadata(Rule{true, true, true}, local_number_of_traces);

    input_file.read_param(
        traces_begin, local_number_of_traces, &trace_metadata, 0);

    for (size_t i = 0; i < trace_metadata.size(); i++) {
        trace_metadata.set_index(i, Trace_metadata_key::ltn, i);
        trace_metadata.set_index(i, Trace_metadata_key::gtn, traces_begin + i);
    }

    // Sort the trace metadata globally and get the sorted order.
    // The indices received is the order data should be read from the input file
    // to write a contiguous sorted chunk.
    auto sorted_order =
        sort(piol.get(), trace_metadata, get_comp(sort_type), false);

    // Reading must be done with monotonically increasing offsets on each
    // process.
    // The `sorted_order` array is unlikely to have the indices to read in
    // order.
    // We must therefore sort the `sorted_order` indices to be able to read
    // them.
    // We must then generate a permutation that will "un-sort" our data after
    // reading.

    // Note: de_sorted_order indices are local indices
    auto de_sorted_order = std::vector<size_t>{};
    de_sorted_order.resize(sorted_order.size());
    std::iota(de_sorted_order.begin(), de_sorted_order.end(), size_t(0));

    // This generates the mapping m[i] = j, where i is the index the trace will
    // be read into, and j is the index the trace should be written
    std::sort(
        de_sorted_order.begin(), de_sorted_order.end(),
        [&](auto a, auto b) { return sorted_order[a] < sorted_order[b]; });

    std::sort(sorted_order.begin(), sorted_order.end());


    // Setup trace storage
    std::vector<Trace_value> trace_data;
    trace_data.resize(local_number_of_traces * samples_per_trace);

    // TODO: setup trace_medatada so it's just copying the raw trace data

    input_file.read_trace_non_contiguous(
        local_number_of_traces, sorted_order.data(), trace_data.data(),
        &trace_metadata, 0);


    // De-sort the traces
    {
        Trace_metadata tmp_metadata(Rule{true, true, true}, 1);
        for (size_t src = 0; src < de_sorted_order.size(); src++) {
            // Keep swapping current src with target src until the current src
            // is the target src
            while (src != de_sorted_order[src]) {
                assert(src < de_sorted_order.size());
                const size_t dst = de_sorted_order[src];
                assert(dst < de_sorted_order.size());

                // Swap metadata
                tmp_metadata.copy_entries(0, trace_metadata, dst);
                trace_metadata.copy_entries(dst, trace_metadata, src);
                trace_metadata.copy_entries(src, tmp_metadata, 0);

                // Swap data
                const auto pos_data_begin =
                    trace_data.begin() + (src * samples_per_trace);
                const auto dst_data_begin =
                    trace_data.begin() + (dst * samples_per_trace);
                std::swap_ranges(
                    pos_data_begin, pos_data_begin + samples_per_trace,
                    dst_data_begin);

                // Swap de_sorted_order
                std::swap(de_sorted_order[src], de_sorted_order[dst]);
            }
        }
    }


    Output_file_segy output_file(piol, output_filename);

    output_file.write_text(input_file.read_text());
    output_file.write_ns(input_file.read_ns());
    output_file.write_nt(input_file.read_nt());
    output_file.write_sample_interval(input_file.read_sample_interval());

    output_file.write_trace(
        traces_begin, local_number_of_traces, trace_data.data(),
        &trace_metadata, 0);

    piol->assert_ok();

    return 0;
}
