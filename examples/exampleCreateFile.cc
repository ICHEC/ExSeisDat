///
/// Example: exampleCreateFile.cc    {#examples_exampleCreateFile_cc}
/// =============================
///
/// @todo DOCUMENT ME - Finish documenting example.
///
/// This example shows how to make a new file with the file api is complete.
///

#include "exseis/piol.hh"

#include <assert.h>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>


int main()
{
    // Set output file name, number of traces, number of samples per trace, and
    // sampling rate
    std::string name       = "CreateFileOutputCPP";
    size_t nt              = 8000;
    size_t ns              = 4000;
    double sample_interval = .01;

    // Initialize the library by creating a communicator object
    auto communicator = exseis::Communicator_mpi{MPI_COMM_WORLD};

    // Create new SEGY file
    exseis::Output_file_segy file(exseis::IO_driver_mpi{
        communicator, name, exseis::File_mode_mpi::Write});

    auto dec = exseis::block_decomposition(
        nt, communicator.get_num_rank(), communicator.get_rank());
    size_t offset = dec.global_offset;
    size_t lnt    = dec.local_size;

    // Write some header parameters
    file.write_samples_per_trace(ns);
    file.write_number_of_traces(nt);
    file.write_sample_interval(sample_interval);
    file.write_text("Test file\n");

    auto trace_metadata_available = file.trace_metadata_available();
    trace_metadata_available.erase(exseis::Trace_metadata_key::raw);

    // Set and write some trace parameters
    exseis::Trace_metadata trace_metadata(trace_metadata_available, lnt);
    for (size_t j = 0; j < lnt; j++) {
        float k = offset + j;
        trace_metadata.set_floating_point(
            j, exseis::Trace_metadata_key::source_x, 1600.0 + k);
        trace_metadata.set_floating_point(
            j, exseis::Trace_metadata_key::source_y, 2400.0 + k);
        trace_metadata.set_floating_point(
            j, exseis::Trace_metadata_key::receiver_x, 100000.0 + k);
        trace_metadata.set_floating_point(
            j, exseis::Trace_metadata_key::receiver_y, 3000000.0 + k);
        trace_metadata.set_floating_point(
            j, exseis::Trace_metadata_key::cdp_x, 10000.0 + k);
        trace_metadata.set_floating_point(
            j, exseis::Trace_metadata_key::cdp_y, 4000.0 + k);
        trace_metadata.set_integer(j, exseis::Trace_metadata_key::il, 2400 + k);
        trace_metadata.set_integer(j, exseis::Trace_metadata_key::xl, 1600 + k);
        trace_metadata.set_integer(
            j, exseis::Trace_metadata_key::file_trace_index, offset + j);
    }
    file.write_metadata(offset, lnt, trace_metadata);

    // Set and write some traces
    std::vector<float> trace_data(lnt * ns);
    for (size_t j = 0; j < lnt * ns; j++) {
        trace_data[j] = float(offset * ns + j);
    }
    file.write_data(offset, lnt, trace_data.data());

    return 0;
}
