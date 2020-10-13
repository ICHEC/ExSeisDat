////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date January 2017
/// @brief
/// @details This file contains the I/O related portions of the 4d Binning
///          utility.
////////////////////////////////////////////////////////////////////////////////

#include "4dio.hh"
#include "sglobal.hh"

#include "exseis/piol/file/Input_file_segy.hh"
#include "exseis/piol/file/Output_file_segy.hh"
#include "exseis/piol/io_driver/IO_driver_mpi.hh"
#include "exseis/piol/segy/utils.hh"

#include "exseis/piol/operations/sort.hh"

#include <assert.h>
#include <iostream>
#include <numeric>


namespace exseis {
namespace apps {
inline namespace fourdbin {

// TODO: Integration candidate
// TODO: Simple IME optimisation: Contig Read all headers, sort, random write
//       all headers to order, IME shuffle, contig read all headers again
std::unique_ptr<Coords> get_coords(
    const Communicator_mpi& communicator, std::string name, bool ixline)
{
    auto time = MPI_Wtime();
    Input_file_segy file(
        IO_driver_mpi{communicator, name, File_mode_mpi::Read});

    auto dec = block_decomposition(
        file.read_number_of_traces(), communicator.get_num_rank(),
        communicator.get_rank());

    size_t offset = dec.global_offset;
    size_t lnt    = dec.local_size;

    auto coords = std::make_unique<Coords>(lnt, ixline);
    assert(coords.get());

    auto trace_metadata_available = file.trace_metadata_available();
    std::map<Trace_metadata_key, Trace_metadata_info> interesting_metadata;
    interesting_metadata[Trace_metadata_key::gtn] = {Type::UInt64};
    interesting_metadata[Trace_metadata_key::source_x] =
        trace_metadata_available[Trace_metadata_key::source_x];

    /* These two lines are for some basic memory limitation calculations. In
     * future versions of the PIOL this will be handled internally and in a more
     * accurate way. User Story S-01490. The for loop a few lines below reads
     * the trace parameters in batches because of this memory limit.
     */
    size_t biggest = communicator.max(lnt);
    size_t max =
        2LU * 1024LU * 1024LU * 1024LU - 4LU * biggest * sizeof(Floating_point);

    // Collective I/O requries an equal number of MPI-IO calls on every process
    // in exactly the same sequence as each other.
    // If not, the code will deadlock. Communication is done to ensure we
    // balance  out the correct number of redundant calls
    size_t extra = biggest / max + (biggest % max > 0 ? 1 : 0)
                   - (lnt / max + (lnt % max > 0 ? 1 : 0));

    Trace_metadata trace_metadata(interesting_metadata, lnt);
    for (size_t i = 0; i < lnt; i += max) {
        size_t rblock = (i + max < lnt ? max : lnt - i);

        // WARNING: Treat ReadSEGY like the internal API for using a
        //         non-exposed function
        file.read_metadata(offset + i, rblock, trace_metadata, i);

        for (size_t j = 0; j < rblock; j++) {
            trace_metadata.set_index(
                i + j, Trace_metadata_key::gtn, offset + i + j);
        }
    }

    // Any extra read_metadata calls the particular process needs
    for (size_t i = 0; i < extra; i++) {
        file.read_metadata();
    }
    cmsg(communicator, "get_coords sort");

    auto trlist = sort(
        communicator, trace_metadata,
        [](const Trace_metadata& trace_metadata, const size_t i,
           const size_t j) -> bool {
            const auto x_src_i = trace_metadata.get_floating_point(
                i, Trace_metadata_key::source_x);
            const auto x_src_j = trace_metadata.get_floating_point(
                j, Trace_metadata_key::source_x);

            if (x_src_i < x_src_j) {
                return true;
            }
            if (x_src_i > x_src_j) {
                return false;
            }

            // x_src_i == x_src_j

            return trace_metadata.get_index(i, Trace_metadata_key::gtn)
                   < trace_metadata.get_index(j, Trace_metadata_key::gtn);
        },
        false);

    cmsg(communicator, "get_coords post-sort I/O");

    ////////////////////////////////////////////////////////////////////////////

    const auto crule = ([&]() {
        using Key = Trace_metadata_key;

        auto trace_metadata_available = file.trace_metadata_available();
        if (ixline) {
            return decltype(trace_metadata_available){
                {Key::source_x, trace_metadata_available[Key::source_x]},
                {Key::source_y, trace_metadata_available[Key::source_y]},
                {Key::receiver_x, trace_metadata_available[Key::receiver_x]},
                {Key::receiver_y, trace_metadata_available[Key::receiver_y]},
                {Key::il, trace_metadata_available[Key::il]},
                {Key::xl, trace_metadata_available[Key::xl]}};
        }

        return decltype(trace_metadata_available){
            {Key::source_x, trace_metadata_available[Key::source_x]},
            {Key::source_y, trace_metadata_available[Key::source_y]},
            {Key::receiver_x, trace_metadata_available[Key::receiver_x]},
            {Key::receiver_y, trace_metadata_available[Key::receiver_y]}};
    }());


    {
        Trace_metadata trace_metadata_2(std::move(crule), std::min(lnt, max));
        for (size_t i = 0; i < lnt; i += max) {
            size_t rblock = (i + max < lnt ? max : lnt - i);

            auto sortlist = get_sort_index(rblock, trlist.data() + i);
            auto orig     = sortlist;
            for (size_t j = 0; j < sortlist.size(); j++) {
                sortlist[j] = trlist[i + sortlist[j]];
            }

            file.read_metadata_non_contiguous(
                rblock, sortlist.data(), trace_metadata_2);

            for (size_t j = 0; j < rblock; j++) {
                coords->x_src[i + orig[j]] =
                    trace_metadata_2.get_floating_point(
                        j, Trace_metadata_key::source_x);
                coords->y_src[i + orig[j]] =
                    trace_metadata_2.get_floating_point(
                        j, Trace_metadata_key::source_y);

                coords->x_rcv[i + orig[j]] =
                    trace_metadata_2.get_floating_point(
                        j, Trace_metadata_key::receiver_x);
                coords->y_rcv[i + orig[j]] =
                    trace_metadata_2.get_floating_point(
                        j, Trace_metadata_key::receiver_y);

                coords->tn[i + orig[j]] = trlist[i + orig[j]];
            }
            for (size_t j = 0; ixline && j < rblock; j++) {
                coords->il[i + orig[j]] =
                    trace_metadata_2.get_integer(j, Trace_metadata_key::il);
                coords->xl[i + orig[j]] =
                    trace_metadata_2.get_integer(j, Trace_metadata_key::xl);
            }
        }

        // Any extra read_metadata calls the particular process needs
        for (size_t i = 0; i < extra; i++) {
            file.read_metadata_non_contiguous();
        }
    }

    // This barrier is necessary so that cmsg doesn't store an old
    // MPI_Wtime().
    communicator.barrier();
    cmsg(
        communicator, "Read sets of coordinates from file " + name + " in "
                          + std::to_string(MPI_Wtime() - time) + " seconds");

    return coords;
}

// TODO: Have a mechanism to change from one Trace_metadata representation to
// another? This is an output related function and doesn't change the core
// algorithm.
void output_non_mono(
    const Communicator_mpi& communicator,
    std::string dname,
    std::string sname,
    std::vector<size_t>& list,
    std::vector<fourd_t>& minrs,
    const bool print_dsr)
{
    auto time = MPI_Wtime();

    Input_file_segy src(
        IO_driver_mpi{communicator, sname, File_mode_mpi::Read});
    Output_file_segy dst(
        IO_driver_mpi{communicator, dname, File_mode_mpi::Write});

    auto trace_metadata_available = src.trace_metadata_available();

    decltype(trace_metadata_available) interesting_metadata = {
        {Trace_metadata_key::raw,
         trace_metadata_available[Trace_metadata_key::raw]}};

    // TODO: Add adding custom Blob_parsers to Output_file_segy
    // // Note: Set to TimeScal for OpenCPS viewing of dataset.
    // // OpenCPS is restrictive on what locations can be used
    // // as scalars.
    // if (print_dsr) {
    //     rule.add_segy_float(
    //         Trace_metadata_key::dsdr, segy::Trace_header_offsets::SrcMeas,
    //         segy::Trace_header_offsets::TimeScal);
    // }

    size_t ns      = src.read_samples_per_trace();
    size_t lnt     = list.size();
    size_t offset  = 0;
    size_t biggest = 0;
    size_t sz      = 0;
    {
        auto nts = communicator.gather(std::vector<size_t>{lnt});
        for (size_t i = 0; i < nts.size(); i++) {
            if (i == communicator.get_rank()) {
                offset = sz;
            }
            sz += nts[i];
            biggest = std::max(biggest, nts[i]);
        }
    }

    size_t max   = 1024LU * 1024LU * 1024LU;
    size_t extra = biggest / max + (biggest % max > 0 ? 1 : 0)
                   - (lnt / max + (lnt % max > 0 ? 1 : 0));

    dst.write_text("ExSeisDat 4d-bin file.\n");
    dst.write_number_of_traces(sz);
    dst.write_sample_interval(src.read_sample_interval());
    dst.write_samples_per_trace(ns);

    Trace_metadata trace_metadata(interesting_metadata, std::min(lnt, max));
    std::vector<Trace_value> trc(ns * std::min(lnt, max));

    communicator.barrier();
    for (size_t i = 0; i < communicator.get_num_rank(); i++) {
        if (i == communicator.get_rank()) {
            std::cout << "rank " << communicator.get_rank() << " loops "
                      << lnt / max + extra << std::endl;
        }
        communicator.barrier();
    }

    for (size_t i = 0; i < lnt; i += max) {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        src.read_non_monotonic(rblock, &list[i], trc.data(), trace_metadata);
        if (print_dsr) {
            for (size_t j = 0; j < rblock; j++) {
                trace_metadata.set_floating_point(
                    j, Trace_metadata_key::dsdr, minrs[i + j]);
            }
        }
        dst.write(offset + i, rblock, trc.data(), trace_metadata);
    }

    for (size_t i = 0; i < extra; i++) {
        src.read_non_contiguous();
        dst.write();
    }

    communicator.barrier();
    cmsg(
        communicator, "Output " + sname + " to " + dname + " in "
                          + std::to_string(MPI_Wtime() - time) + " seconds");
}

}  // namespace fourdbin
}  // namespace apps
}  // namespace exseis