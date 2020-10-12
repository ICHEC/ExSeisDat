////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief This function takes one or more files as input and produces a new
///          file or files which contain all traces with identical ns and
///          increment.
////////////////////////////////////////////////////////////////////////////////

#include "exseis/piol.hh"
#include "exseis/utils.hh"

#include <CLI11.hpp>

#include <iostream>

#include <assert.h>
#include <glob.h>
#include <unistd.h>


/// @brief Parsed CLI options for the concatenate program
struct Options {
    /// The input files to concatenate
    std::vector<std::string> input_filenames;

    /// The output file to concatenate into
    std::string output_filename;

    /// The message to write in the file header
    std::string message = "Concatenated with ExSeisDat";
};


/// @brief Parse the CLI for the concatenate program
///
/// @param[in] argc The argc value from main
/// @param[in] argv The argv value from main
///
/// @returns The parsed CLI options for the concatenate program
Options parse_cli(int argc, char* argv[])
{
    CLI::App app{"Concatenate SEG-Y files"};

    Options options;

    app.add_option("input,-i,--input", options.input_filenames, "Input file(s)")
        ->required();
    app.add_option("-o,--output", options.output_filename, "Output file")
        ->required();

    app.add_option("-m,--message", options.message, "Header message");

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

    const auto& input_filenames = options.input_filenames;
    const auto& output_filename = options.output_filename;
    const auto& message         = options.message;

    auto communicator = exseis::Communicator_mpi{MPI_COMM_WORLD};

    size_t output_samples_per_trace = 0;
    size_t output_number_of_traces  = 0;

    exseis::Output_file_segy output_file(exseis::IO_driver_mpi{
        communicator, output_filename, exseis::File_mode_mpi::Write});

    output_file.write_text(message);

    bool first = true;
    for (auto& input_filename : input_filenames) {
        exseis::Input_file_segy input_file(exseis::IO_driver_mpi{
            communicator, input_filename, exseis::File_mode_mpi::Read});

        const size_t samples_per_trace = input_file.read_samples_per_trace();
        const size_t number_of_traces  = input_file.read_number_of_traces();
        const float sample_interval    = input_file.read_sample_interval();

        // We need to be careful: each trace must have the same samples per
        // trace!
        // We'll just assume the first file has the correct samples per trace
        // and sample interval.
        if (first) {
            output_samples_per_trace = samples_per_trace;

            output_file.write_samples_per_trace(samples_per_trace);
            output_file.write_sample_interval(sample_interval);

            first = false;
        }

        // NOTE: We're only checking for the correct samples per trace!
        if (output_samples_per_trace != samples_per_trace) {
            if (communicator.get_rank() == 0) {
                std::cerr
                    << "WARNING: input samples per trace different from output samples per trace. Skipping: "
                    << input_filename << '\n';
            }

            continue;  // skip to next file
        }

        // Add the input number of traces to the total number of traces so far.
        // We assume write_number_of_traces will resize the file as
        // necessary........
        const size_t output_traces_begin = output_number_of_traces;
        output_number_of_traces += number_of_traces;
        output_file.write_number_of_traces(output_number_of_traces);


        output_file.sync();  // Ensure update to number of traces is synced


        // Find the number of traces to read on each process
        auto decomposition = exseis::block_decomposition(
            number_of_traces, communicator.get_num_rank(),
            communicator.get_rank());
        const auto input_offset = decomposition.global_offset;
        const auto input_size   = decomposition.local_size;

        // NOTE: Each input SEG-Y file must fit into distributed memory.
        // TODO: Read input SEG-Y files in chunks

        // Trace storage
        std::vector<exseis::Trace_value> trace_data;
        trace_data.resize(input_size * samples_per_trace);

        exseis::Trace_metadata trace_metadata(
            exseis::Rule{true, true, true}, input_size);

        // Read and write the traces!
        input_file.read(
            input_offset, input_size, trace_data.data(), trace_metadata);

        output_file.write(
            output_traces_begin + input_offset, input_size, trace_data.data(),
            trace_metadata);
    }

    return 0;
}
