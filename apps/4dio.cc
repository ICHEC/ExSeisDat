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

#include "exseisdat/piol/configuration/ExSeis.hh"
#include "exseisdat/piol/file/Input_file_segy.hh"
#include "exseisdat/piol/file/Output_file_segy.hh"
#include "exseisdat/piol/segy/utils.hh"

#include "exseisdat/piol/operations/sort.hh"

#include <assert.h>
#include <numeric>

using namespace exseis::utils;

namespace exseis {
namespace piol {
namespace four_d {

// TODO: Integration candidate
// TODO: Simple IME optimisation: Contig Read all headers, sort, random write
//       all headers to order, IME shuffle, contig read all headers again
std::unique_ptr<Coords> get_coords(
    std::shared_ptr<ExSeisPIOL> piol, std::string name, bool ixline)
{
    auto time = MPI_Wtime();
    Input_file_segy file(piol, name);
    piol->assert_ok();

    auto dec = block_decomposition(
        file.read_number_of_traces(), piol->comm->get_num_rank(),
        piol->comm->get_rank());

    size_t offset = dec.global_offset;
    size_t lnt    = dec.local_size;

    auto coords = std::make_unique<Coords>(lnt, ixline);
    assert(coords.get());
    auto rule = Rule(std::initializer_list<Trace_metadata_key>{
        Trace_metadata_key::gtn, Trace_metadata_key::x_src});
    /* These two lines are for some basic memory limitation calculations. In
     * future versions of the PIOL this will be handled internally and in a more
     * accurate way. User Story S-01490. The for loop a few lines below reads
     * the trace parameters in batches because of this memory limit.
     */
    size_t biggest = piol->comm->max(lnt);
    size_t memlim  = 2LU * 1024LU * 1024LU * 1024LU
                    - 4LU * biggest * sizeof(exseis::utils::Floating_point);
    size_t max =
        memlim
        / (rule.memory_usage_per_header() + segy::segy_trace_header_size());

    // Collective I/O requries an equal number of MPI-IO calls on every process
    // in exactly the same sequence as each other.
    // If not, the code will deadlock. Communication is done to ensure we
    // balance  out the correct number of redundant calls
    size_t extra = biggest / max + (biggest % max > 0 ? 1 : 0)
                   - (lnt / max + (lnt % max > 0 ? 1 : 0));

    Trace_metadata trace_metadata(std::move(rule), lnt);
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
    cmsg(piol.get(), "get_coords sort");

    auto trlist = sort(
        piol.get(), trace_metadata,
        [](const Trace_metadata& trace_metadata, const size_t i,
           const size_t j) -> bool {
            const auto x_src_i =
                trace_metadata.get_floating_point(i, Trace_metadata_key::x_src);
            const auto x_src_j =
                trace_metadata.get_floating_point(j, Trace_metadata_key::x_src);

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

    cmsg(piol.get(), "get_coords post-sort I/O");

    ////////////////////////////////////////////////////////////////////////////

    const auto crule = ([ixline]() {
        if (ixline) {
            return Rule(std::initializer_list<Trace_metadata_key>{
                Trace_metadata_key::x_src, Trace_metadata_key::y_src,
                Trace_metadata_key::x_rcv, Trace_metadata_key::y_rcv,
                Trace_metadata_key::il, Trace_metadata_key::xl});
        }

        return Rule(std::initializer_list<Trace_metadata_key>{
            Trace_metadata_key::x_src, Trace_metadata_key::y_src,
            Trace_metadata_key::x_rcv, Trace_metadata_key::y_rcv});
    }());

    max = memlim
          / (crule.memory_usage_per_header() + segy::segy_trace_header_size()
             + 2LU * sizeof(size_t));

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
                        j, Trace_metadata_key::x_src);
                coords->y_src[i + orig[j]] =
                    trace_metadata_2.get_floating_point(
                        j, Trace_metadata_key::y_src);

                coords->x_rcv[i + orig[j]] =
                    trace_metadata_2.get_floating_point(
                        j, Trace_metadata_key::x_rcv);
                coords->y_rcv[i + orig[j]] =
                    trace_metadata_2.get_floating_point(
                        j, Trace_metadata_key::y_rcv);

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
    piol->comm->barrier();
    cmsg(
        piol.get(), "Read sets of coordinates from file " + name + " in "
                        + std::to_string(MPI_Wtime() - time) + " seconds");

    return coords;
}

// TODO: Have a mechanism to change from one Trace_metadata representation to
// another? This is an output related function and doesn't change the core
// algorithm.
void output_non_mono(
    std::shared_ptr<ExSeisPIOL> piol,
    std::string dname,
    std::string sname,
    std::vector<size_t>& list,
    std::vector<fourd_t>& minrs,
    const bool print_dsr)
{
    auto time = MPI_Wtime();
    auto rule = Rule(
        std::initializer_list<Trace_metadata_key>{Trace_metadata_key::Copy});

    // Note: Set to TimeScal for OpenCPS viewing of dataset.
    // OpenCPS is restrictive on what locations can be used
    // as scalars.
    if (print_dsr) {
        rule.add_segy_float(
            Trace_metadata_key::dsdr, segy::Trace_header_offsets::SrcMeas,
            segy::Trace_header_offsets::TimeScal);
    }

    Input_file_segy src(piol, sname);
    Output_file_segy dst(piol, dname);
    piol->assert_ok();

    size_t ns      = src.read_samples_per_trace();
    size_t lnt     = list.size();
    size_t offset  = 0;
    size_t biggest = 0;
    size_t sz      = 0;
    {
        auto nts = piol->comm->gather(std::vector<size_t>{lnt});
        for (size_t i = 0; i < nts.size(); i++) {
            if (i == piol->comm->get_rank()) {
                offset = sz;
            }
            sz += nts[i];
            biggest = std::max(biggest, nts[i]);
        }
    }

    size_t memlim = 1024LU * 1024LU * 1024LU;
    size_t max =
        memlim / (4LU * segy::segy_trace_size(ns) + 4LU * rule.extent());
    size_t extra = biggest / max + (biggest % max > 0 ? 1 : 0)
                   - (lnt / max + (lnt % max > 0 ? 1 : 0));

    dst.write_text("ExSeisDat 4d-bin file.\n");
    dst.write_number_of_traces(sz);
    dst.write_sample_interval(src.read_sample_interval());
    dst.write_samples_per_trace(ns);

    Trace_metadata trace_metadata(std::move(rule), std::min(lnt, max));
    std::vector<exseis::utils::Trace_value> trc(ns * std::min(lnt, max));

    piol->comm->barrier();
    for (size_t i = 0; i < piol->comm->get_num_rank(); i++) {
        if (i == piol->comm->get_rank()) {
            std::cout << "rank " << piol->comm->get_rank() << " loops "
                      << lnt / max + extra << std::endl;
        }
        piol->comm->barrier();
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

    piol->comm->barrier();
    cmsg(
        piol.get(), "Output " + sname + " to " + dname + " in "
                        + std::to_string(MPI_Wtime() - time) + " seconds");
}

}  // namespace four_d
}  // namespace piol
}  // namespace exseis
