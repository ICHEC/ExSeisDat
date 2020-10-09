///
/// Example: example1.cc    {#examples_example1_cc}
/// ====================
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

int main(int argc, char** argv)
{
    std::string opt = "o:";  // TODO: uses a GNU extension
    std::string name;
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
        if (c == 'o') {
            name = optarg;
        }
        else {
            std::cerr << "One of the command line arguments is invalid"
                      << std::endl;
            return -1;
        }
    }
    assert(!name.empty());

    // Initialise the PIOL by creating an ExSeisPIOL object
    auto piol = ExSeis::make();

    // Create a SEGY file object
    Output_file_segy file(piol, name);

    // nt is the number of traces, ns the number of samples per trace
    size_t nt = 40000;
    size_t ns = 1000;
    // inc is the increment step between traces (microseconds)
    double sample_interval = 4.0;

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
