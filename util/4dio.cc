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

#include "exseisdat/piol/ExSeis.hh"
#include "exseisdat/piol/ReadSEGY.hh"
#include "exseisdat/piol/WriteSEGY.hh"
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
    ReadSEGY file(piol, name);
    piol->assert_ok();

    auto dec = block_decomposition(
        file.read_nt(), piol->comm->get_num_rank(), piol->comm->get_rank());

    size_t offset = dec.global_offset;
    size_t lnt    = dec.local_size;

    auto coords = std::make_unique<Coords>(lnt, ixline);
    assert(coords.get());
    auto rule = Rule(std::initializer_list<Meta>{Meta::gtn, Meta::x_src});
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

    Trace_metadata prm(std::move(rule), lnt);
    for (size_t i = 0; i < lnt; i += max) {
        size_t rblock = (i + max < lnt ? max : lnt - i);

        // WARNING: Treat ReadSEGY like the internal API for using a
        //         non-exposed function
        file.read_param(offset + i, rblock, &prm, i);

        for (size_t j = 0; j < rblock; j++) {
            prm.set_index(i + j, Meta::gtn, offset + i + j);
        }
    }

    // Any extra read_param calls the particular process needs
    for (size_t i = 0; i < extra; i++) {
        file.read_param(size_t(0), size_t(0), &prm);
    }
    cmsg(piol.get(), "get_coords sort");

    auto trlist = sort(
        piol.get(), prm,
        [](const Trace_metadata& prm, const size_t i, const size_t j) -> bool {
            const auto x_src_i = prm.get_floating_point(i, Meta::x_src);
            const auto x_src_j = prm.get_floating_point(j, Meta::x_src);

            if (x_src_i < x_src_j) {
                return true;
            }
            if (x_src_i > x_src_j) {
                return false;
            }

            // x_src_i == x_src_j

            return prm.get_index(i, Meta::gtn) < prm.get_index(j, Meta::gtn);
        },
        false);

    cmsg(piol.get(), "get_coords post-sort I/O");

    ////////////////////////////////////////////////////////////////////////////

    const auto crule = ([ixline]() {
        if (ixline) {
            return Rule(std::initializer_list<Meta>{Meta::x_src, Meta::y_src,
                                                    Meta::x_rcv, Meta::y_rcv,
                                                    Meta::il, Meta::xl});
        }

        return Rule(std::initializer_list<Meta>{Meta::x_src, Meta::y_src,
                                                Meta::x_rcv, Meta::y_rcv});
    }());

    max = memlim
          / (crule.memory_usage_per_header() + segy::segy_trace_header_size()
             + 2LU * sizeof(size_t));

    {
        Trace_metadata prm2(std::move(crule), std::min(lnt, max));
        for (size_t i = 0; i < lnt; i += max) {
            size_t rblock = (i + max < lnt ? max : lnt - i);

            auto sortlist = get_sort_index(rblock, trlist.data() + i);
            auto orig     = sortlist;
            for (size_t j = 0; j < sortlist.size(); j++) {
                sortlist[j] = trlist[i + sortlist[j]];
            }

            file.read_param_non_contiguous(rblock, sortlist.data(), &prm2);

            for (size_t j = 0; j < rblock; j++) {
                coords->x_src[i + orig[j]] =
                    prm2.get_floating_point(j, Meta::x_src);
                coords->y_src[i + orig[j]] =
                    prm2.get_floating_point(j, Meta::y_src);

                coords->x_rcv[i + orig[j]] =
                    prm2.get_floating_point(j, Meta::x_rcv);
                coords->y_rcv[i + orig[j]] =
                    prm2.get_floating_point(j, Meta::y_rcv);

                coords->tn[i + orig[j]] = trlist[i + orig[j]];
            }
            for (size_t j = 0; ixline && j < rblock; j++) {
                coords->il[i + orig[j]] = prm2.get_integer(j, Meta::il);
                coords->xl[i + orig[j]] = prm2.get_integer(j, Meta::xl);
            }
        }

        // Any extra read_param calls the particular process needs
        for (size_t i = 0; i < extra; i++) {
            file.read_param_non_contiguous(0LU, nullptr, &prm2);
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
    auto rule = Rule(std::initializer_list<Meta>{Meta::Copy});

    // Note: Set to TimeScal for OpenCPS viewing of dataset.
    // OpenCPS is restrictive on what locations can be used
    // as scalars.
    if (print_dsr) {
        rule.add_segy_float(Meta::dsdr, Tr::SrcMeas, Tr::TimeScal);
    }

    ReadSEGY src(piol, sname);
    WriteSEGY dst(piol, dname);
    piol->assert_ok();

    size_t ns      = src.read_ns();
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
    dst.write_nt(sz);
    dst.write_sample_interval(src.read_sample_interval());
    dst.write_ns(ns);

    Trace_metadata prm(std::move(rule), std::min(lnt, max));
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
        src.read_trace_non_monotonic(rblock, &list[i], trc.data(), &prm);
        if (print_dsr) {
            for (size_t j = 0; j < rblock; j++) {
                prm.set_floating_point(j, Meta::dsdr, minrs[i + j]);
            }
        }
        dst.write_trace(offset + i, rblock, trc.data(), &prm);
    }

    for (size_t i = 0; i < extra; i++) {
        src.read_trace_non_contiguous(size_t(0), nullptr, trc.data(), &prm);
        dst.write_trace(size_t(0), size_t(0), trc.data(), &prm);
    }

    piol->comm->barrier();
    cmsg(
        piol.get(), "Output " + sname + " to " + dname + " in "
                        + std::to_string(MPI_Wtime() - time) + " seconds");
}

}  // namespace four_d
}  // namespace piol
}  // namespace exseis
