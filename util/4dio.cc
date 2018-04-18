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

#include "ExSeisDat/PIOL/ExSeis.hh"
#include "ExSeisDat/PIOL/ReadDirect.hh"
#include "ExSeisDat/PIOL/WriteDirect.hh"
#include "ExSeisDat/PIOL/param_utils.hh"
#include "ExSeisDat/PIOL/segy_utils.hh"

#include "ExSeisDat/PIOL/operations/sort.hh"

#include <assert.h>
#include <numeric>

using namespace exseis::utils;

namespace exseis {
namespace PIOL {
namespace FOURD {

// TODO: Integration candidate
// TODO: Simple IME optimisation: Contig Read all headers, sort, random write
//       all headers to order, IME shuffle, contig read all headers again
std::unique_ptr<Coords> getCoords(
  std::shared_ptr<ExSeisPIOL> piol, std::string name, bool ixline)
{
    auto time = MPI_Wtime();
    ReadDirect file(piol, name);
    piol->isErr();

    auto dec = block_decomposition(
      file->readNt(), piol->comm->getNumRank(), piol->comm->getRank());

    size_t offset = dec.global_offset;
    size_t lnt    = dec.local_size;

    auto coords = std::make_unique<Coords>(lnt, ixline);
    assert(coords.get());
    auto rule = std::make_shared<Rule>(
      std::initializer_list<Meta>{PIOL_META_gtn, PIOL_META_xSrc});
    /* These two lines are for some basic memory limitation calculations. In
     * future versions of the PIOL this will be handled internally and in a more
     * accurate way. User Story S-01490. The for loop a few lines below reads
     * the trace parameters in batches because of this memory limit.
     */
    size_t biggest = piol->comm->max(lnt);
    size_t memlim  = 2LU * 1024LU * 1024LU * 1024LU
                    - 4LU * biggest * sizeof(exseis::utils::Floating_point);
    size_t max = memlim / (rule->paramMem() + SEGY_utils::getMDSz());

    // Collective I/O requries an equal number of MPI-IO calls on every process
    // in exactly the same sequence as each other.
    // If not, the code will deadlock. Communication is done to ensure we
    // balance  out the correct number of redundant calls
    size_t extra =
      biggest / max - lnt / max + (biggest % max > 0) - (lnt % max > 0);

    Param prm(rule, lnt);
    for (size_t i = 0; i < lnt; i += max) {
        size_t rblock = (i + max < lnt ? max : lnt - i);

        // WARNING: Treat ReadDirect like the internal API for using a
        //         non-exposed function
        file->readParam(offset + i, rblock, &prm, i);

        for (size_t j = 0; j < rblock; j++)
            param_utils::setPrm(i + j, PIOL_META_gtn, offset + i + j, &prm);
    }

    // Any extra readParam calls the particular process needs
    for (size_t i = 0; i < extra; i++)
        file.readParam(size_t(0), size_t(0), nullptr);
    cmsg(piol.get(), "getCoords sort");

    auto trlist = sort(
      piol.get(), &prm,
      [](const Param* prm, const size_t i, const size_t j) -> bool {
          return (
            param_utils::getPrm<exseis::utils::Floating_point>(
              i, PIOL_META_xSrc, prm)
                < param_utils::getPrm<exseis::utils::Floating_point>(
                    j, PIOL_META_xSrc, prm) ?
              true :
              param_utils::getPrm<exseis::utils::Floating_point>(
                i, PIOL_META_xSrc, prm)
                  == param_utils::getPrm<exseis::utils::Floating_point>(
                       j, PIOL_META_xSrc, prm)
                && param_utils::getPrm<size_t>(i, PIOL_META_gtn, prm)
                     < param_utils::getPrm<size_t>(j, PIOL_META_gtn, prm));
      },
      false);

    cmsg(piol.get(), "getCoords post-sort I/O");

    ////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<Rule> crule;
    if (ixline)
        crule = std::make_shared<Rule>(std::initializer_list<Meta>{
          PIOL_META_xSrc, PIOL_META_ySrc, PIOL_META_xRcv, PIOL_META_yRcv,
          PIOL_META_il, PIOL_META_xl});
    else
        crule = std::make_shared<Rule>(std::initializer_list<Meta>{
          PIOL_META_xSrc, PIOL_META_ySrc, PIOL_META_xRcv, PIOL_META_yRcv});

    max = memlim
          / (crule->paramMem() + SEGY_utils::getMDSz() + 2LU * sizeof(size_t));

    {
        Param prm2(crule, std::min(lnt, max));
        for (size_t i = 0; i < lnt; i += max) {
            size_t rblock = (i + max < lnt ? max : lnt - i);

            auto sortlist = getSortIndex(rblock, trlist.data() + i);
            auto orig     = sortlist;
            for (size_t j = 0; j < sortlist.size(); j++)
                sortlist[j] = trlist[i + sortlist[j]];

            file.readParamNonContiguous(rblock, sortlist.data(), &prm2);

            for (size_t j = 0; j < rblock; j++) {
                coords->xSrc[i + orig[j]] =
                  param_utils::getPrm<exseis::utils::Floating_point>(
                    j, PIOL_META_xSrc, &prm2);
                coords->ySrc[i + orig[j]] =
                  param_utils::getPrm<exseis::utils::Floating_point>(
                    j, PIOL_META_ySrc, &prm2);
                coords->xRcv[i + orig[j]] =
                  param_utils::getPrm<exseis::utils::Floating_point>(
                    j, PIOL_META_xRcv, &prm2);
                coords->yRcv[i + orig[j]] =
                  param_utils::getPrm<exseis::utils::Floating_point>(
                    j, PIOL_META_yRcv, &prm2);
                coords->tn[i + orig[j]] = trlist[i + orig[j]];
            }
            for (size_t j = 0; ixline && j < rblock; j++) {
                coords->il[i + orig[j]] =
                  param_utils::getPrm<exseis::utils::Integer>(
                    j, PIOL_META_il, &prm2);
                coords->xl[i + orig[j]] =
                  param_utils::getPrm<exseis::utils::Integer>(
                    j, PIOL_META_xl, &prm2);
            }
        }
    }

    // Any extra readParam calls the particular process needs
    for (size_t i = 0; i < extra; i++) {
        file.readParamNonContiguous(0LU, nullptr, nullptr);
    }

    // This barrier is necessary so that cmsg doesn't store an old MPI_Wtime().
    piol->comm->barrier();
    cmsg(
      piol.get(), "Read sets of coordinates from file " + name + " in "
                    + std::to_string(MPI_Wtime() - time) + " seconds");

    return coords;
}

// TODO: Have a mechanism to change from one Param representation to another?
// This is an output related function and doesn't change the core algorithm.
void outputNonMono(
  std::shared_ptr<ExSeisPIOL> piol,
  std::string dname,
  std::string sname,
  std::vector<size_t>& list,
  std::vector<fourd_t>& minrs,
  const bool printDsr)
{
    auto time = MPI_Wtime();
    auto rule =
      std::make_shared<Rule>(std::initializer_list<Meta>{PIOL_META_COPY});

    // Note: Set to TimeScal for OpenCPS viewing of dataset.
    // OpenCPS is restrictive on what locations can be used
    // as scalars.
    if (printDsr) {
        rule->addSEGYFloat(PIOL_META_dsdr, PIOL_TR_SrcMeas, PIOL_TR_TimeScal);
    }

    ReadDirect src(piol, sname);
    WriteDirect dst(piol, dname);
    piol->isErr();

    size_t ns      = src.readNs();
    size_t lnt     = list.size();
    size_t offset  = 0;
    size_t biggest = 0;
    size_t sz      = 0;
    {
        auto nts = piol->comm->gather(std::vector<size_t>{lnt});
        for (size_t i = 0; i < nts.size(); i++) {
            if (i == piol->comm->getRank()) offset = sz;
            sz += nts[i];
            biggest = std::max(biggest, nts[i]);
        }
    }

    size_t memlim = 1024LU * 1024LU * 1024LU;
    size_t max =
      memlim / (4LU * SEGY_utils::getDOSz(ns) + 4LU * rule->extent());
    size_t extra =
      biggest / max - lnt / max + (biggest % max > 0) - (lnt % max > 0);

    dst.writeText("ExSeisDat 4d-bin file.\n");
    dst.writeNt(sz);
    dst.writeInc(src.readInc());
    dst.writeNs(ns);

    Param prm(rule, std::min(lnt, max));
    std::vector<exseis::utils::Trace_value> trc(ns * std::min(lnt, max));

    piol->comm->barrier();
    for (size_t i = 0; i < piol->comm->getNumRank(); i++) {
        if (i == piol->comm->getRank())
            std::cout << "rank " << piol->comm->getRank() << " loops "
                      << lnt / max + extra << std::endl;
        piol->comm->barrier();
    }

    for (size_t i = 0; i < lnt; i += max) {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        src.readTraceNonMonotonic(rblock, &list[i], trc.data(), &prm);
        if (printDsr)
            for (size_t j = 0; j < rblock; j++)
                param_utils::setPrm(j, PIOL_META_dsdr, minrs[i + j], &prm);
        dst.writeTrace(offset + i, rblock, trc.data(), &prm);
    }

    for (size_t i = 0; i < extra; i++) {
        src.readTraceNonContiguous(size_t(0), nullptr, nullptr, nullptr);
        dst.writeTrace(size_t(0), size_t(0), nullptr, nullptr);
    }

    piol->comm->barrier();
    cmsg(
      piol.get(), "Output " + sname + " to " + dname + " in "
                    + std::to_string(MPI_Wtime() - time) + " seconds");
}

}  // namespace FOURD
}  // namespace PIOL
}  // namespace exseis
