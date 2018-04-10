///
/// Example: exampleCreateFile.cc    {#examples_exampleCreateFile_cc}
/// =============================
///
/// @todo DOCUMENT ME - Finish documenting example.
///
/// This example shows how to make a new file with the file api is complete.
///

#include "ExSeisDat/PIOL.hh"

#include <assert.h>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

using namespace PIOL;

void createFile(std::string name, size_t nt, size_t ns, double inc)
{
    // Initialize PIOL by creating an ExSeisPIOL object
    auto piol = ExSeis::New();

    // Create new SEGY file
    WriteDirect file(piol, name);

    auto dec      = decompose_range(nt, piol->getNumRank(), piol->getRank());
    size_t offset = dec.global_offset;
    size_t lnt    = dec.local_size;

    // Write some header parameters
    file.writeNs(ns);
    file.writeNt(nt);
    file.writeInc(inc);
    file.writeText("Test file\n");

    // Set and write some trace parameters
    Param prm(lnt);
    for (size_t j = 0; j < lnt; j++) {
        float k = offset + j;
        param_utils::setPrm(j, PIOL_META_xSrc, 1600.0 + k, &prm);
        param_utils::setPrm(j, PIOL_META_ySrc, 2400.0 + k, &prm);
        param_utils::setPrm(j, PIOL_META_xRcv, 100000.0 + k, &prm);
        param_utils::setPrm(j, PIOL_META_yRcv, 3000000.0 + k, &prm);
        param_utils::setPrm(j, PIOL_META_xCmp, 10000.0 + k, &prm);
        param_utils::setPrm(j, PIOL_META_yCmp, 4000.0 + k, &prm);
        param_utils::setPrm(j, PIOL_META_il, 2400 + k, &prm);
        param_utils::setPrm(j, PIOL_META_xl, 1600 + k, &prm);
        param_utils::setPrm(j, PIOL_META_tn, offset + j, &prm);
    }
    file.writeParam(offset, lnt, &prm);

    // Set and write some traces
    std::vector<float> trc(lnt * ns);
    for (size_t j = 0; j < lnt * ns; j++)
        trc[j] = float(offset * ns + j);
    file.writeTrace(offset, lnt, trc.data());
}


int main(void)
{
    // Set output file name, number of traces, number of samples per trace, and
    // sampling rate
    std::string name = "CreateFileOutputCPP";
    size_t nt        = 8000;
    size_t ns        = 4000;
    double inc       = .01;

    createFile(name, nt, ns, inc);
    return 0;
}
