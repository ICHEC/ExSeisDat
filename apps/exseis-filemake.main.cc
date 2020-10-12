#include "sglobal.hh"

#include "exseis/piol/file/Output_file_segy.hh"
#include "exseis/piol/io_driver/IO_driver_mpi.hh"
#include "exseis/piol/segy/utils.hh"
#include "exseis/utils/communicator/Communicator_mpi.hh"

#include <assert.h>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>


void write_contig(
    exseis::Output_file& file,
    size_t offset,
    size_t nt,
    size_t ns,
    size_t lnt,
    size_t extra,
    size_t max)
{
    float fhalf = float(nt * ns) / 2.0;
    float off   = float(nt * ns) / 4.0;
    long nhalf  = nt / 2;
    exseis::Trace_metadata trace_metadata(max);
    std::vector<float> trc(max * ns);
    for (size_t i = 0; i < lnt; i += max) {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        for (size_t j = 0; j < rblock; j++) {
            float k = nhalf - std::abs(-nhalf + long(offset + i + j));
            trace_metadata.set_floating_point(
                j, exseis::Trace_metadata_key::x_src, 1600.0 + k);
            trace_metadata.set_floating_point(
                j, exseis::Trace_metadata_key::y_src, 2400.0 + k);
            trace_metadata.set_floating_point(
                j, exseis::Trace_metadata_key::x_rcv, 100000.0 + k);
            trace_metadata.set_floating_point(
                j, exseis::Trace_metadata_key::y_rcv, 3000000.0 + k);
            trace_metadata.set_floating_point(
                j, exseis::Trace_metadata_key::xCmp, 10000.0 + k);
            trace_metadata.set_floating_point(
                j, exseis::Trace_metadata_key::yCmp, 4000.0 + k);
            trace_metadata.set_integer(
                j, exseis::Trace_metadata_key::il, 2400 + k);
            trace_metadata.set_integer(
                j, exseis::Trace_metadata_key::xl, 1600 + k);
            trace_metadata.set_integer(
                j, exseis::Trace_metadata_key::tn, offset + i + j);
        }
        for (size_t j = 0; j < trc.size(); j++) {
            trc[j] =
                fhalf - std::abs(-fhalf + float((offset + i) * ns + j)) - off;
        }
        file.write(offset + i, rblock, trc.data(), trace_metadata);
    }
    for (size_t j = 0; j < extra; j++) {
        file.write();
    }
}

void write_random(
    const exseis::Communicator& communicator,
    exseis::Output_file& file,
    size_t nt,
    size_t ns,
    size_t lnt,
    size_t extra,
    size_t max)
{
    float fhalf = float(nt * ns) / 2.0;
    float off   = float(nt * ns) / 4.0;
    long nhalf  = nt / 2;

    std::vector<size_t> offset(lnt);
    auto num = communicator.gather<size_t>(lnt);

    size_t rank     = communicator.get_rank();
    size_t num_rank = communicator.get_num_rank();

    size_t offcount = 0;
    size_t t        = 0;

    // Work out offsets
    while (num[rank] > 0 && offcount < nt) {
        for (size_t j = 0; j < num_rank; j++) {
            if (j != rank) {
                if (num[j] != 0) {
                    num[j]--;
                    offcount++;
                }
            }
            else {
                num[rank]--;
                offset[t++] = offcount++;
            }
        }
    }

    for (size_t i = 0; i < lnt; i += max) {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        exseis::Trace_metadata trace_metadata(rblock);
        std::vector<float> trc(rblock * ns);

        for (size_t j = 0; j < rblock; j++) {
            float k = nhalf - std::abs(-nhalf + long(offset[i + j]));
            trace_metadata.set_floating_point(
                j, exseis::Trace_metadata_key::x_src, 1600.0 + k);
            trace_metadata.set_floating_point(
                j, exseis::Trace_metadata_key::y_src, 2400.0 + k);
            trace_metadata.set_floating_point(
                j, exseis::Trace_metadata_key::x_rcv, 100000.0 + k);
            trace_metadata.set_floating_point(
                j, exseis::Trace_metadata_key::y_rcv, 3000000.0 + k);
            trace_metadata.set_floating_point(
                j, exseis::Trace_metadata_key::xCmp, 10000.0 + k);
            trace_metadata.set_floating_point(
                j, exseis::Trace_metadata_key::yCmp, 4000.0 + k);
            trace_metadata.set_integer(
                j, exseis::Trace_metadata_key::il, 2400 + k);
            trace_metadata.set_integer(
                j, exseis::Trace_metadata_key::xl, 1600 + k);
            trace_metadata.set_integer(
                j, exseis::Trace_metadata_key::tn, offset[i + j]);
        }
        for (size_t j = 0; j < trc.size(); j++) {
            trc[j] =
                fhalf - std::abs(-fhalf + float((offset[i]) * ns + j)) - off;
        }
        file.write_non_contiguous(
            rblock, &offset[i], trc.data(), trace_metadata);
    }

    for (size_t j = 0; j < extra; j++) {
        file.write_non_contiguous();
    }
}

void file_make(
    bool lob,
    bool random,
    const std::string name,
    size_t max,
    size_t ns,
    size_t nt,
    exseis::Floating_point sample_interval)
{
    auto communicator = exseis::Communicator_mpi{MPI_COMM_WORLD};

    exseis::Output_file_segy file(exseis::IO_driver_mpi{
        communicator, name, exseis::File_mode_mpi::Write});

    file.write_samples_per_trace(ns);
    file.write_number_of_traces(nt);
    file.write_sample_interval(sample_interval);
    file.write_text("Test file\n");

    size_t offset  = 0;
    size_t lnt     = 0;
    size_t biggest = 0;

    if (lob) {
        auto dec = exseis::apps::lobdecompose(
            communicator, nt, communicator.get_num_rank(),
            communicator.get_rank());
        offset  = dec[0];
        lnt     = dec[1];
        biggest = dec[2];
    }
    else {
        auto dec = exseis::block_decomposition(
            nt, communicator.get_num_rank(), communicator.get_rank());

        offset  = dec.global_offset;
        lnt     = dec.local_size;
        biggest = communicator.max(lnt);
    }

    // TODO: Add memusage for Trace_metadata
    max /=
        (exseis::segy::segy_trace_size(ns)
         + exseis::segy::segy_trace_data_size(ns) + sizeof(size_t));
    size_t extra = biggest / max + (biggest % max > 0 ? 1 : 0)
                   - (lnt / max + (lnt % max > 0 ? 1 : 0));
    if (random) {
        write_random(communicator, file, nt, ns, lnt, extra, max);
    }
    else {
        write_contig(file, offset, nt, ns, lnt, extra, max);
    }
}

int main(int argc, char** argv)
{
    std::string name;
    size_t ns                              = 0;
    size_t nt                              = 0;
    size_t max                             = 0;
    exseis::Floating_point sample_interval = 0.0;
    bool lob                               = false;
    bool random                            = false;

    if (argc <= 1) {
        std::cout
            << "Options: filemake -o \"name.segy\" -s <ns> -t <nt> -m <mem(MiB)> -i <inc>\n";
        return EXIT_FAILURE;
    }

    // TODO: document these arguments:
    //           -o output file
    //           -s number of samples
    //           -t number of traces
    //           -m max
    //           -i increment
    //           -l lobsided
    //           -r 'random' offsets
    std::string opt = "s:t:m:o:i:lr";  // TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
        switch (c) {
            case 'o':
                name = optarg;
                break;

            case 's':
                ns = std::stoul(optarg);
                break;

            case 't':
                nt = std::stoul(optarg);
                break;

            case 'm':
                max = std::stoul(optarg);
                break;

            case 'i':
                sample_interval = std::stod(optarg);
                break;

            case 'l':
                lob = true;
                break;

            case 'r':
                random = true;
                break;

            default:
                std::cerr << "One of the command line arguments is invalid\n";
                break;
        }
    }

    assert(!name.empty() && max != 0 && sample_interval != 0.0);
    max *= 1024 * 1024;
    file_make(lob, random, name, max, ns, nt, sample_interval);

    return 0;
}
