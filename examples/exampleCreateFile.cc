///
/// Example: exampleCreateFile.cc    {#examples_exampleCreateFile_cc}
/// =============================
///
/// @todo DOCUMENT ME - Finish documenting example.
///
/// This example shows how to make a new file with the file api is complete.
///

#include "exseisdat/piol.hh"

#include <assert.h>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

using namespace exseis::utils;
using namespace exseis::piol;

int main()
{
    // Set output file name, number of traces, number of samples per trace, and
    // sampling rate
    std::string name       = "CreateFileOutputCPP";
    size_t nt              = 8000;
    size_t ns              = 4000;
    double sample_interval = .01;

    // Initialize PIOL by creating an ExSeisPIOL object
    auto piol = ExSeis::make();

    // Create new SEGY file
    Output_file_segy file(piol, name);

    auto dec = block_decomposition(nt, piol->get_num_rank(), piol->get_rank());
    size_t offset = dec.global_offset;
    size_t lnt    = dec.local_size;

    // Write some header parameters
    file.write_samples_per_trace(ns);
    file.write_number_of_traces(nt);
    file.write_sample_interval(sample_interval);
    file.write_text("Test file\n");

    // Set and write some trace parameters
    Trace_metadata trace_metadata(lnt);
    for (size_t j = 0; j < lnt; j++) {
        float k = offset + j;
        trace_metadata.set_floating_point(
            j, Trace_metadata_key::x_src, 1600.0 + k);
        trace_metadata.set_floating_point(
            j, Trace_metadata_key::y_src, 2400.0 + k);
        trace_metadata.set_floating_point(
            j, Trace_metadata_key::x_rcv, 100000.0 + k);
        trace_metadata.set_floating_point(
            j, Trace_metadata_key::y_rcv, 3000000.0 + k);
        trace_metadata.set_floating_point(
            j, Trace_metadata_key::xCmp, 10000.0 + k);
        trace_metadata.set_floating_point(
            j, Trace_metadata_key::yCmp, 4000.0 + k);
        trace_metadata.set_integer(j, Trace_metadata_key::il, 2400 + k);
        trace_metadata.set_integer(j, Trace_metadata_key::xl, 1600 + k);
        trace_metadata.set_integer(j, Trace_metadata_key::tn, offset + j);
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
