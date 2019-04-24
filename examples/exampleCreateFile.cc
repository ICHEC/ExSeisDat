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
    WriteSEGY file(piol, name);

    auto dec = block_decomposition(nt, piol->get_num_rank(), piol->get_rank());
    size_t offset = dec.global_offset;
    size_t lnt    = dec.local_size;

    // Write some header parameters
    file.write_ns(ns);
    file.write_nt(nt);
    file.write_sample_interval(sample_interval);
    file.write_text("Test file\n");

    // Set and write some trace parameters
    Trace_metadata prm(lnt);
    for (size_t j = 0; j < lnt; j++) {
        float k = offset + j;
        prm.set_floating_point(j, Meta::x_src, 1600.0 + k);
        prm.set_floating_point(j, Meta::y_src, 2400.0 + k);
        prm.set_floating_point(j, Meta::x_rcv, 100000.0 + k);
        prm.set_floating_point(j, Meta::y_rcv, 3000000.0 + k);
        prm.set_floating_point(j, Meta::xCmp, 10000.0 + k);
        prm.set_floating_point(j, Meta::yCmp, 4000.0 + k);
        prm.set_integer(j, Meta::il, 2400 + k);
        prm.set_integer(j, Meta::xl, 1600 + k);
        prm.set_integer(j, Meta::tn, offset + j);
    }
    file.write_param(offset, lnt, &prm);

    // Set and write some traces
    std::vector<float> trc(lnt * ns);
    for (size_t j = 0; j < lnt * ns; j++) {
        trc[j] = float(offset * ns + j);
    }
    file.write_trace(offset, lnt, trc.data());

    return 0;
}
