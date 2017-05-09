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
};
}
#endif
