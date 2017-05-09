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
#include "set/share.hh"
#include "share/misc.hh"

#warning Move to cache.cc
#include "file/dynsegymd.hh"

namespace PIOL {
struct CacheElem
{
    FileDeque desc;
    std::unique_ptr<File::Param> prm;
    std::vector<trace_t> trc;
    CacheElem(FileDeque & desc_, std::unique_ptr<File::Param> prm_)
    {
        desc = desc_;
        prm = std::move(prm_);
    }
    CacheElem(FileDeque & desc_, std::vector<trace_t> & trc_, std::unique_ptr<File::Param> prm_ = nullptr)
    {
        trc = std::move(trc_);
        desc = desc_;
        prm = std::move(prm_);
    }
};

class Cache
{
    std::vector<CacheElem> cache;
    Piol piol;
    public :
    Cache(Piol piol_) : piol(piol_) {}

    File::Param * cachePrm(std::shared_ptr<File::Rule> rule, FileDeque & desc);

    trace_t * cacheTrc(FileDeque & desc);

    bool checkPrm(FileDeque & desc)
    {
        auto it = std::find_if(cache.begin(), cache.end(), [desc] (const CacheElem & elem) -> bool { return elem.desc == desc && elem.prm != nullptr; });
        return it != cache.end();
    }

    bool checkTrc(FileDeque & desc)
    {
        auto it = std::find_if(cache.begin(), cache.end(), [desc] (const CacheElem & elem) -> bool { return elem.desc == desc && elem.trc.size(); });
        return it != cache.end();
    }

    void flush(FileDeque & desc)
    {
        auto it = std::find_if(cache.begin(), cache.end(), [desc] (const CacheElem & elem) -> bool { return elem.desc == desc; });
        cache.erase(it);
    }

    std::vector<size_t> getOutputTrace(FileDeque & desc, size_t offset, size_t sz, File::Param * prm)
    {
        std::vector<size_t> final(sz);

        auto it = std::find_if(cache.begin(), cache.end(), [desc] (const CacheElem & elem) -> bool { return elem.desc == desc && elem.prm != nullptr; });
        if (it != cache.end())
        {
            auto iprm = it->prm.get();

            size_t loc = 0;
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
