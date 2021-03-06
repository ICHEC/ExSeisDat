////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/Flow/Cache.hh"

#include "ExSeisDat/PIOL/Param.h"
#include "ExSeisDat/PIOL/operations/sort.hh"
#include "ExSeisDat/PIOL/param_utils.hh"

// TODO: Remove when all options implemented
#include <iostream>

using namespace exseis::PIOL;

namespace exseis {
namespace Flow {

// TODO: Generalise this for parameters and traces
std::shared_ptr<TraceBlock> Cache::getCache(
  std::shared_ptr<Rule> rule, FileDeque& desc, bool, bool)
{
    auto it = std::find_if(
      cache.begin(), cache.end(),
      [desc](const CacheElem& elem) -> bool { return elem.desc == desc; });
    if (it == cache.end() || !it->block || !it->block->prm) {
        size_t lnt = 0LU;
        size_t nt  = 0LU;
        for (auto& f : desc) {
            lnt += f->ilst.size();
            nt += f->ifc->readNt();
        }

        size_t off = piol->comm->offset(lnt);
        auto prm   = std::make_unique<Param>(rule, lnt);
        // TODO: Do not make assumptions about Parameter sizes fitting in
        //       memory.
        size_t loff = 0LU;
        size_t c    = 0LU;
        for (auto& f : desc) {
            f->ifc->readParamNonContiguous(
              f->ilst.size(), f->ilst.data(), prm.get(), loff);
            for (size_t i = 0LU; i < f->ilst.size(); i++) {
                param_utils::setPrm(
                  loff + i, PIOL_META_gtn, off + loff + f->ilst[i], prm.get());
                param_utils::setPrm(
                  loff + i, PIOL_META_ltn, f->ilst[i] * desc.size() + c,
                  prm.get());
            }
            c++;
            loff += f->ilst.size();
        }

        if (it == cache.end()) {
            cache.emplace_back(desc, std::move(prm));
            it = cache.end() - 1LU;
        }
        else {
            if (!it->block) {
                it->block = std::make_shared<TraceBlock>();
            }
            it->block->prm = std::move(prm);
        }

        it->block->nt  = nt;
        it->block->ns  = desc[0]->ifc->readNs();
        it->block->inc = desc[0]->ifc->readInc();
    }
    else {
        // TODO: Check if prm is cached and use PIOL_META_COPY
    }
    return it->block;
}

std::vector<size_t> Cache::getOutputTrace(
  FileDeque& desc, size_t offset, size_t sz, Param* prm)
{
    std::vector<size_t> final;

    auto it = std::find_if(
      cache.begin(), cache.end(),
      [desc](const CacheElem& elem) -> bool { return elem.checkPrm(desc); });
    if (it != cache.end()) {
        auto iprm  = it->block->prm.get();
        size_t loc = 0LU;
        final.resize(sz);
        for (size_t i = 0LU; i < desc.size() && loc < offset + sz; i++) {
            size_t fsz  = desc[i]->olst.size();
            size_t nloc = loc + fsz;

            // Some data should be copied from this file
            if (nloc > offset) {
                size_t lsz  = std::min(offset + sz, nloc) - offset;
                size_t foff = offset - loc;
                std::copy(
                  desc[i]->olst.begin() + foff,
                  desc[i]->olst.begin() + foff + lsz,
                  final.begin() + loc - offset);
            }

            loc = nloc;
        }

        std::vector<size_t> sortlist = getSortIndex(sz, final.data());
        for (size_t j = 0LU; j < sz; j++) {
            param_utils::cpyPrm(sortlist[j], iprm, j, prm);
        }
    }

    return final;
}

}  // namespace Flow
}  // namespace exseis
