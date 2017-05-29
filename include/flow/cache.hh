/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date May 2017
 *   \brief
*//*******************************************************************************************/
#ifndef PIOLSETCACHE_INCLUDE_GUARD
#define PIOLSETCACHE_INCLUDE_GUARD
#include <algorithm>
#include "flow/share.hh"
#include "share/misc.hh"

#warning Move to cache.cc
#include "file/dynsegymd.hh"

namespace PIOL {
struct CacheElem
{
    FileDeque desc;
    std::shared_ptr<TraceBlock> block;

    CacheElem(FileDeque & desc_, std::unique_ptr<File::Param> prm_)
    {
        desc = desc_;
        block = std::make_shared<TraceBlock>();
        block->prm = std::move(prm_);
    }
    CacheElem(FileDeque & desc_, std::vector<trace_t> & trc_, std::unique_ptr<File::Param> prm_ = nullptr)
    {
        desc = desc_;
        block = std::make_shared<TraceBlock>();
        block->trc = std::move(trc_);
        block->prm = std::move(prm_);
    }

    bool checkPrm(const FileDeque & desc_) const
    {
        return desc == desc_ && !block && block->prm;
    }

    bool checkTrc(const FileDeque & desc_) const
    {
        return desc == desc_ && !block && block->trc.size();
    }
};

class Cache
{
    std::vector<CacheElem> cache;
    Piol piol;
    public :
    Cache(Piol piol_) : piol(piol_) {}

    std::shared_ptr<TraceBlock> getCache(std::shared_ptr<File::Rule> rule, FileDeque & desc, bool cPrm, bool cTrc);

    std::shared_ptr<TraceBlock> cachePrm(std::shared_ptr<File::Rule> rule, FileDeque & desc)
    {
        return getCache(rule, desc, true, false);
    }

    std::shared_ptr<TraceBlock> cacheTrc(FileDeque & desc)
    {
        return getCache(nullptr, desc, false, true);
    }

    bool checkPrm(FileDeque & desc)
    {
        auto it = std::find_if(cache.begin(), cache.end(), [desc] (const CacheElem & elem) -> bool { return elem.checkPrm(desc); });
        return it != cache.end();
    }

    bool checkTrc(FileDeque & desc)
    {
        auto it = std::find_if(cache.begin(), cache.end(), [desc] (const CacheElem & elem) -> bool { return elem.checkTrc(desc); });
        return it != cache.end();
    }

    void flush(FileDeque & desc)
    {
        auto it = std::find_if(cache.begin(), cache.end(), [desc] (const CacheElem & elem) -> bool { return elem.desc == desc; });
        cache.erase(it);
    }

    std::vector<size_t> getOutputTrace(FileDeque & desc, size_t offset, size_t sz, File::Param * prm)
    {
        std::vector<size_t> final;

        auto it = std::find_if(cache.begin(), cache.end(), [desc] (const CacheElem & elem) -> bool { return elem.checkPrm(desc); });
        if (it != cache.end())
        {
            auto iprm = it->block->prm.get();
            size_t loc = 0;
            final.resize(sz);
            for (size_t i = 0; i < desc.size() && loc < offset+sz; i++)
            {
                size_t fsz = desc[i]->olst.size();
                size_t nloc = loc + fsz;

                if (nloc > offset)  //Some data should be copied from this file
                {
                    size_t lsz = std::min(offset+sz, nloc) - offset;
                    size_t foff = offset - loc;
                    std::copy(desc[i]->olst.begin() + foff, desc[i]->olst.begin() + foff + lsz, final.begin() + loc - offset);
                }

                loc = nloc;
            }

            std::vector<size_t> sortlist = getSortIndex(sz, final.data());
            for (size_t j = 0U; j < sz; j++)
                File::cpyPrm(sortlist[j], iprm, j, prm);
        }
        return final;
    }
};
}
#endif
