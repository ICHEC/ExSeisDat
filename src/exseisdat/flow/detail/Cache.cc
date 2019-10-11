////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/flow/detail/Cache.hh"

#include "exseisdat/piol/metadata/Trace_metadata.hh"
#include "exseisdat/piol/operations/sort_operations/sort.hh"

using namespace exseis::piol;

namespace exseis {
namespace flow {
namespace detail {

// TODO: Generalise this for parameters and traces
std::shared_ptr<Trace_block> Cache::get_cache(
    std::shared_ptr<Rule> rule, FileDeque& desc, bool, bool)
{
    auto it = std::find_if(
        m_cache.begin(), m_cache.end(),
        [desc](const Cache_elem& elem) -> bool { return elem.desc == desc; });
    if (it == m_cache.end() || !it->block || !it->block->prm) {
        size_t lnt = 0LU;
        size_t nt  = 0LU;
        for (auto& f : desc) {
            lnt += f->ilst.size();
            nt += f->ifc->read_nt();
        }

        size_t off = m_piol->comm->offset(lnt);
        auto prm   = std::make_unique<Trace_metadata>(*rule, lnt);
        // TODO: Do not make assumptions about Trace_metadataeter sizes fitting
        // in
        //       memory.
        size_t loff = 0LU;
        size_t c    = 0LU;
        for (auto& f : desc) {
            f->ifc->read_param_non_contiguous(
                f->ilst.size(), f->ilst.data(), prm.get(), loff);
            for (size_t i = 0LU; i < f->ilst.size(); i++) {
                prm->set_index(
                    loff + i, Trace_metadata_key::gtn, off + loff + f->ilst[i]);
                prm->set_index(
                    loff + i, Trace_metadata_key::ltn,
                    f->ilst[i] * desc.size() + c);
            }
            c++;
            loff += f->ilst.size();
        }

        if (it == m_cache.end()) {
            m_cache.emplace_back(desc, std::move(prm));
            it = m_cache.end() - 1LU;
        }
        else {
            if (!it->block) {
                it->block = std::make_shared<Trace_block>();
            }
            it->block->prm = std::move(prm);
        }

        it->block->nt              = nt;
        it->block->ns              = desc[0]->ifc->read_ns();
        it->block->sample_interval = desc[0]->ifc->read_sample_interval();
    }
    else {
        // TODO: Check if prm is cached and use Trace_metadata_key::Copy
    }
    return it->block;
}

std::vector<size_t> Cache::get_output_trace(
    FileDeque& desc, size_t offset, size_t sz, Trace_metadata& prm)
{
    std::vector<size_t> final;

    auto it = std::find_if(
        m_cache.begin(), m_cache.end(),
        [&](const Cache_elem& elem) -> bool { return elem.check_prm(desc); });

    if (it != m_cache.end()) {

        const auto& iprm = *(it->block->prm);

        size_t loc = 0LU;
        final.resize(sz);

        for (size_t i = 0LU; i < desc.size() && loc < offset + sz; i++) {
            size_t fsz  = desc[i]->olst.size();
            size_t nloc = loc + fsz;

            // Some data should be copied from this file
            if (nloc > offset) {
                using Difference = decltype(desc[i]->olst)::difference_type;

                const auto lsz = static_cast<Difference>(
                    std::min(offset + sz, nloc) - offset);
                const auto foff = static_cast<Difference>(offset - loc);

                std::copy(
                    std::next(
                        desc[i]->olst.begin(), static_cast<Difference>(foff)),
                    std::next(
                        desc[i]->olst.begin(),
                        static_cast<Difference>(foff + lsz)),
                    std::next(
                        final.begin(), static_cast<Difference>(loc - offset)));
            }

            loc = nloc;
        }

        std::vector<size_t> sortlist = get_sort_index(sz, final.data());
        for (size_t j = 0LU; j < sz; j++) {
            prm.copy_entries(j, iprm, sortlist[j]);
        }
    }

    return final;
}

}  // namespace detail
}  // namespace flow
}  // namespace exseis
