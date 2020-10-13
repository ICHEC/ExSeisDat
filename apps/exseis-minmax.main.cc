#include "sglobal.hh"

#include "exseis/piol/file/Input_file_segy.hh"
#include "exseis/piol/file/Output_file_segy.hh"
#include "exseis/piol/io_driver/IO_driver_mpi.hh"
#include "exseis/piol/operations/minmax.hh"
#include "exseis/utils/communicator/Communicator_mpi.hh"
#include "exseis/utils/decomposition/block_decomposition.hh"

#include <algorithm>
#include <iostream>
#include <unistd.h>


/*! Read from the input file. Find the min/max  x_src, y_src, x_rcv, y_rcv, xCmp
 *  and yCMP. Write the matching traces to the output file in that order.
 *  @param[in] iname Input file
 *  @param[in] oname Output file
 */
void calc_min(std::string iname, std::string oname)
{
    auto communicator = exseis::Communicator_mpi{MPI_COMM_WORLD};

    exseis::Input_file_segy in(exseis::IO_driver_mpi{
        communicator, iname, exseis::File_mode_mpi::Read});

    auto dec = exseis::block_decomposition(
        in.read_number_of_traces(), communicator.get_num_rank(),
        communicator.get_rank());

    size_t offset = dec.global_offset;
    size_t lnt    = dec.local_size;

    exseis::Trace_metadata trace_metadata(in.trace_metadata_available(), lnt);
    std::vector<exseis::CoordElem> minmax(12U);
    in.read_metadata(offset, lnt, trace_metadata);

    exseis::get_min_max(
        communicator, offset, lnt, exseis::Trace_metadata_key::source_x,
        exseis::Trace_metadata_key::source_y, trace_metadata, minmax.data());
    exseis::get_min_max(
        communicator, offset, lnt, exseis::Trace_metadata_key::receiver_x,
        exseis::Trace_metadata_key::receiver_y, trace_metadata,
        minmax.data() + 4U);
    get_min_max(
        communicator, offset, lnt, exseis::Trace_metadata_key::cdp_x,
        exseis::Trace_metadata_key::cdp_y, trace_metadata, minmax.data() + 8U);

    size_t sz  = (communicator.get_rank() == 0 ? minmax.size() : 0U);
    size_t usz = 0;
    std::vector<size_t> list(sz);
    std::vector<size_t> uniqlist(sz);

    if (sz != 0) {
        for (size_t i = 0U; i < sz; i++) {
            uniqlist[i] = list[i] = minmax[i].num;
        }

        std::sort(uniqlist.begin(), uniqlist.end());
        auto end = std::unique(uniqlist.begin(), uniqlist.end());
        uniqlist.resize(std::distance(uniqlist.begin(), end));

        usz = uniqlist.size();
    }

    exseis::Trace_metadata input_trace_metadata(
        in.trace_metadata_available(), usz);
    in.read_metadata_non_contiguous(usz, uniqlist.data(), input_trace_metadata);


    exseis::Output_file_segy out(exseis::IO_driver_mpi{
        communicator, oname, exseis::File_mode_mpi::Write});

    exseis::Trace_metadata output_trace_metadata(
        out.trace_metadata_available(), sz);
    std::vector<exseis::utils::Trace_value> trace(sz);
    for (size_t i = 0U; i < sz; i++) {
        for (size_t j = 0U; j < usz; j++) {
            if (list[i] == uniqlist[j]) {
                output_trace_metadata.copy_entries(i, input_trace_metadata, j);
                output_trace_metadata.set_integer(
                    i, exseis::Trace_metadata_key::file_trace_index,
                    minmax[i].num);
                trace[i] = exseis::utils::Trace_value(1);
                j        = usz;
            }
        }
    }

    out.write_number_of_traces(sz);
    out.write_samples_per_trace(1U);
    out.write_sample_interval(in.read_sample_interval());
    out.write(0, sz, trace.data(), output_trace_metadata);
}

/* Main function for minmax.
 *  @param[in] argc The number of input strings.
 *  @param[in] argv The array of input strings.
 *  @return zero on success, non-zero on failure
 *  @details Options:
 *           -i \<file\> : input file name
 *           -o \<file\> : output file name
 */
int main(int argc, char** argv)
{
    std::string iname;
    std::string oname;

    std::string opt = "i:o:";  // TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
        switch (c) {
            case 'i':
                iname = optarg;
                break;

            case 'o':
                oname = optarg;
                break;

            default:
                std::cerr << "One of the command line arguments is invalid.\n";
                return -1;
        }
    }

    if (iname.empty() || oname.empty()) {
        std::cerr << "Invalid arguments given.\n";
        std::cerr << "Arguments: -i for input file, -o for output file\n";
        return -1;
    }

    calc_min(iname, oname);

    return 0;
}