/// @cond GENERATING_MANPAGE
///
/// @page exseis-extract-metadata exseis-extract-metadata
/// @section name NAME
///     exseis-extract-metadata -- Extract metadata from a SEG-Y file
///
/// @section synopsis SYNOPSIS
///     exseis-extract-metadata [options] -i INPUT_FILENAME -o OUTPUT_FILENAME
///                             -p Metadata1[,Metadata2],Metadata3[...]]]
///
/// @section description DESCRIPTION
///     The exseis-extract-metadata utility reads metadata from SEG-Y files
///     and writes them to a CSV file in parallel. Use mpiexec to launch
///     a parallel job.
///
///     The options are as follows:
///
///     \par -i INPUT_FILENAME
///         The name of the SEG-Y file to read from.
///
///     \par -o OUTPUT_FILENAME
///         The name of the CSV file to write to.
///
///     \par -p Metadata1[,Metadata2[,Metadata3[...]]]
///         A comma separated list of the parameter names to read from the
///         input file.
///
///    \par -n TRACE_STRIDE_SIZE
///         Read every \"TRACE_STRIDE_SIZE\"th trace
///
///    \par -m MAX_NUMBER_OF_TRACES_IN_MEMORY
///         The maximum number of traces to read in and store in memory at once.
///
/// @section exit_status EXIT STATUS
///     exseis-extract-metadata returns 0 on success and >0 on failure.
///
/// @section author AUTHOR
///     The ExSeisDat Team
///
/// @endcond
//           // GENERATING_MANPAGE


#include "exseisdat/piol/configuration/ExSeis.hh"
#include "exseisdat/piol/file/Input_file_segy.hh"
#include "exseisdat/piol/io_driver/IO_driver_mpi.hh"
#include "exseisdat/piol/metadata/Trace_metadata.hh"
#include "exseisdat/piol/metadata/rules/Rule.hh"
#include "exseisdat/utils/decomposition/block_decomposition.hh"

#include <CLI11.hpp>

#include <algorithm>
#include <array>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

using namespace exseis::utils;
using namespace exseis::piol;


/// A map from metadata key names to matadata keys.
const auto meta_name_to_meta_map = std::map<std::string, Trace_metadata_key>{
    {"x_src", Trace_metadata_key::x_src},
    {"y_src", Trace_metadata_key::y_src},
    {"x_rcv", Trace_metadata_key::x_rcv},
    {"y_rcv", Trace_metadata_key::y_rcv},
    {"coordinate_scalar", Trace_metadata_key::coordinate_scalar}};


/// @brief Return a fixed-width string representing the given Trace_metadata_key key at the
///        given trace index from the given Trace_metadata object.
/// @param[in] trace_metadata    The trace metadata object storing the trace
///                              metadata to format.
/// @param[in] local_trace_index The index into the trace_metadata object to
///                              look up.
/// @param[in] meta              The metadata key to look up.
/// @returns A fixed width string representing
/// @code
///     trace_metadata.get_/*integer/floating_point*/(local_trace_index, meta);
/// @endcode
///
std::string formatted_meta_value(
    const Trace_metadata& trace_metadata,
    size_t local_trace_index,
    Trace_metadata_key meta);

int main(int argc, char* argv[])
{
    // Command line settable values
    std::string input_filename;
    std::string output_filename;
    std::vector<std::string> meta_names;
    std::vector<Trace_metadata_key> metas;
    size_t trace_stride_size              = 1;
    size_t max_number_of_traces_in_memory = std::numeric_limits<size_t>::max();

    CLI::App app(
        "Extract SEG-Y metadata, and output it to CSV.",
        "exseis-extract-metadata");

    // Input and output filenames
    app.add_option(
           "-i,--input", input_filename,
           "The SEG-Y file to read the metadata from.")
        ->required()
        ->check(CLI::ExistingFile)
        ->type_name("FILENAME");

    app.add_option(
           "-o,--output", output_filename,
           "The CSV file to write the metadata to.")
        ->required()
        ->check(CLI::NonexistentPath)
        ->type_name("FILENAME");

    // Trace stride and chunking options
    app.add_option(
           "-n,--nth-trace", trace_stride_size, "Read every n'th trace.", true)
        ->type_name("N");
    app.add_option(
           "-m,--max-traces-in-memory", max_number_of_traces_in_memory,
           "The maximum number of traces to keep in memory on any process.")
        ->type_name("N_MAX");

    // Metadata names
    app.add_option(
           "metadata_names,--meta", meta_names,
           "A space-separated list of metadata names to extract.")
        ->type_name("")
        ->required();

    // Print supported metadata names and exit.
    app.add_flag_function("--list-supported-metadata", [](size_t /*unused*/) {
        std::cout << "Supported metadata names:\n";
        for (auto it = meta_name_to_meta_map.cbegin();
             it != meta_name_to_meta_map.cend(); ++it) {
            std::cout << "    " << it->first << '\n';
        }

        throw CLI::Success();
    });

    // Get the metadata names and keys from the input parameters
    app.callback([&] {
        for (const auto& meta_name : meta_names) {
            auto meta_key_it = meta_name_to_meta_map.find(meta_name);

            if (meta_key_it == meta_name_to_meta_map.end()) {
                throw CLI::ParseError(
                    "Unknown metadata name: \"" + meta_name + "\"!",
                    EXIT_FAILURE);
            }

            metas.push_back(meta_key_it->second);
        }
    });


    CLI11_PARSE(app, argc, argv);


    // Finished command line parsing //////////////////////////////////////////


    // Initialize the ExSeisPIOL library and open the input SEGY file and the
    // output CSV file.
    auto piol = ExSeis::make();
    const Input_file_segy input_file(piol, input_filename);
    IO_driver_mpi output_file(
        output_filename, File_mode_mpi::Write, MPI_COMM_WORLD, piol->log);


    // Build the header for the CSV file in the form
    //     # meta_1, meta_2, meta_3
    std::string csv_header;
    csv_header += "# ";
    for (size_t meta_name_index = 0; meta_name_index < meta_names.size();
         meta_name_index++) {
        csv_header += meta_names[meta_name_index];

        const bool is_last = meta_name_index + 1 == meta_names.size();
        csv_header += is_last ? "\n" : ", ";
    }


    // Output the CSV header from the master process
    if (piol->get_rank() == 0) {
        output_file.write(0, csv_header.size(), csv_header.data());
    }
    else {
        output_file.write(0, 0, nullptr);
    }


    //
    // Decompose the input traces over the processes.
    // We want to read every nth trace in the file, and decompose that
    // list of every nth trace across the processes.
    //

    size_t global_number_of_traces = input_file.read_number_of_traces();

    // Get a block decomposition of the traces in the file
    const auto decomposition = block_decomposition(
        global_number_of_traces, piol->get_num_rank(), piol->get_rank());

    const size_t global_block_trace_start     = decomposition.global_offset;
    const size_t local_block_number_of_traces = decomposition.local_size;


    // We only want to output every `trace_stride_size` traces, so we need
    // to find the first of these in our local block, and the number of
    // them in the local block.
    //
    // We'll be describing every `trace_stride_size`th trace as an "aligned"
    // trace. i.e. its index is a multiple of `trace_stride_size`.

    const auto round_up_divide = [](auto a, auto b) { return (a + b - 1) / b; };

    const auto round_up_to_alignment = [=](auto v) {
        const auto alignment = trace_stride_size;
        return round_up_divide(v, alignment) * alignment;
    };

    // Round up from block start to an aligned trace
    const size_t global_trace_start =
        round_up_to_alignment(global_block_trace_start);

    // Get a first-aligned-past-the-end index.
    // The alignment is needed because we'll be dividing by `trace_stride_size`
    // later. An un-aligned index would give us an off-by-1 error.
    const size_t aligned_trace_end = round_up_to_alignment(
        global_block_trace_start
        + round_up_to_alignment(local_block_number_of_traces));

    // Find the total number of aligned traces in the local block
    const size_t local_number_of_traces = ([=] {
        // If the first aligned trace is outside the local block, the local
        // number of aligned traces is 0.
        const auto global_block_trace_end =
            global_block_trace_start + local_block_number_of_traces;

        if (global_trace_start >= global_block_trace_end) {
            return size_t(0);
        }

        return (aligned_trace_end - global_trace_start) / trace_stride_size;
    }());

    // Since max_number_of_traces_in_memory may be less than
    // local_number_of_traces, we'll likely need to loop in chunks of
    // size max_number_of_traces_in_memory.

    size_t max_chunk_size =
        std::min(local_number_of_traces, max_number_of_traces_in_memory);

    const size_t local_number_of_chunks = ([=] {
        if (max_chunk_size == 0) {
            return size_t(0);
        }

        return round_up_divide(local_number_of_traces, max_chunk_size);
    }());

    // Since not every process will have the same number of chunks, but
    // reading and writing is collective, we need to make sure everyone
    // makes the same number of calls.
    const size_t global_number_of_chunks = piol->max(local_number_of_chunks);

    // Setup the local trace metadata storage
    const Rule rule(metas);
    Trace_metadata trace_metadata(rule, max_chunk_size);

    // A list of the trace indices for each chunk.
    // This will be used / reused on each iteration of the loop.
    auto trace_indices = std::vector<size_t>();
    trace_indices.resize(max_chunk_size);

    for (size_t chunk_index = 0; chunk_index < global_number_of_chunks;
         chunk_index++) {
        // Set up local_chunk_start, local_chunk_end, and current_chunk_size
        // so that
        //     chunk_index < local_number_of_chunks
        //     --> current_chunk_size = max_chunk_size
        // and
        //     chunk_index > local_number_of_chunks
        //     --> current_chunk_size = 0.
        // and
        //     local_chunk_start + current_chunk_size <= local_number_of_traces
        // This will result in read and write functions being passed array
        // sizes of zero, so they'll participate in collective operations
        // without actually writing anything.

        const size_t local_chunk_start =
            std::min(chunk_index * max_chunk_size, local_number_of_traces);

        const size_t local_chunk_end = std::min(
            (chunk_index + 1) * max_chunk_size, local_number_of_traces);

        const size_t current_chunk_size = local_chunk_end - local_chunk_start;


        // Generate the trace indices for this chunk
        for (size_t i = 0; i < current_chunk_size; i++) {
            trace_indices[i] = global_trace_start
                               + (local_chunk_start + i) * trace_stride_size;
        }

        // Read the trace metadata into memory
        input_file.read_metadata_non_contiguous(
            current_chunk_size, trace_indices.data(), trace_metadata);


        // Generate the local chunk of the CSV on each process
        std::string csv_body;

        for (size_t stored_trace_index = 0;
             stored_trace_index < current_chunk_size; stored_trace_index++) {

            for (size_t meta_index = 0; meta_index < metas.size();
                 meta_index++) {
                csv_body += formatted_meta_value(
                    trace_metadata, stored_trace_index, metas[meta_index]);

                // Add ", " after the entry if it's not the last on the line.
                // Otherwise, add \n.
                const bool is_last = meta_index + 1 == metas.size();
                csv_body += is_last ? "\n" : ", ";
            }
        }

        // Get the size of a single line. Every line *should* be the same size!
        // If local_number_of_traces == 0 then csv_body.size() == 0, so we
        // can avoid division by zero by taking the max with 1.
        const size_t csv_line_size =
            csv_body.size() / std::max<size_t>(current_chunk_size, 1);
        assert(csv_body.size() % std::max<size_t>(current_chunk_size, 1) == 0);

        // Find the offset in the file to the trace of index global_trace_start.
        const size_t global_file_offset = csv_header.size()
                                          + csv_line_size * global_trace_start
                                          + local_chunk_start;


        // Write the local chunk to the CSV file
        output_file.write(global_file_offset, csv_body.size(), csv_body.data());
    }

    return EXIT_SUCCESS;
}


// We want every line of data in the CSV file to be the same size so we
// can write it in parallel. To do this, we need a way to turn a meta
// entry into a well formatted string.
std::string formatted_meta_value(
    const Trace_metadata& trace_metadata,
    size_t local_trace_index,
    Trace_metadata_key meta)
{
    // Float and integer output should be 16 characters long
    const char* float_format   = "%16.5f";
    const char* integer_format = "%16d";
    std::array<char, 17> char_array{};

    switch (trace_metadata.entry_type(meta)) {
        case Type::Double:
        case Type::Float: {
            auto size = std::snprintf(
                char_array.data(), char_array.size(), float_format,
                trace_metadata.get_floating_point(local_trace_index, meta));
            (void)size;

            assert(size == char_array.size() - 1);
        } break;

        case Type::Int64:
        case Type::UInt64:
        case Type::Int32:
        case Type::UInt32:
        case Type::Int16:
        case Type::UInt16:
        case Type::Int8:
        case Type::UInt8: {
            auto size = std::snprintf(
                char_array.data(), char_array.size(), integer_format,
                trace_metadata.get_integer(local_trace_index, meta));

            (void)size;
            assert(size == char_array.size() - 1);
        } break;

        case Type::Index:
        case Type::Copy:
        default:
            assert(
                false
                && "Error in formatted_meta: Unexpected exseis::utils::Type");
    }

    return std::string(char_array.begin(), char_array.end());
}
