////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date May 2017
/// @brief
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLSETCACHE_INCLUDE_GUARD
#define PIOLSETCACHE_INCLUDE_GUARD

#include "flow/share.hh"
#include "share/misc.hh"

#include <algorithm>

namespace PIOL {

/*! A structure to store cache elements of traces and parameters corresponding
 *  to a collection of files
 */
struct CacheElem {
    /// A deque of unique pointers to file descriptors
    FileDeque desc;
    /// The cached data
    std::shared_ptr<TraceBlock> block;

    /*! Construct the cache element with the given parameter structure.
     *  @param[in] desc_ A deque of unique pointers to file descriptors
     *  @param[inout] prm_ A unique_ptr to the parameter structure. The cache
     *                element takes ownership.
     */
    CacheElem(FileDeque& desc_, std::unique_ptr<File::Param> prm_)
    {
        desc       = desc_;
        block      = std::make_shared<TraceBlock>();
        block->prm = std::move(prm_);
    }

    /*! Construct the cache element with the given parameter structure.
     *  @param[in] desc_ A deque of unique pointers to file descriptors
     *  @param[in] trc_ A vector of traces for caching.
     *  @param[inout] prm_ A unique_ptr to the parameter structure. The cache
     *                element takes ownership if it exists.
     */
    CacheElem(
      FileDeque& desc_,
      std::vector<trace_t>& trc_,
      std::unique_ptr<File::Param> prm_ = nullptr)
    {
        desc       = desc_;
        block      = std::make_shared<TraceBlock>();
        block->trc = std::move(trc_);
        block->prm = std::move(prm_);
    }

    /*! Check if the given element has cached parameters
     *  @param[in] desc_ A deque of unique pointers to file descriptors
     *  @return Return true if the parameters are cached.
     */
    bool checkPrm(const FileDeque& desc_) const
    {
        return desc == desc_ && !block && block->prm;
    }

    /*! Check if the given element has cached traces
     *  @param[in] desc_ A deque of unique pointers to file descriptors
     *  @return Return true if the traces are cached.
     */
    bool checkTrc(const FileDeque& desc_) const
    {
        return desc == desc_ && !block && block->trc.size();
    }
};

/*! The class which holds all cache elements.
 */
class Cache {
    /// A vector of cache elements
    std::vector<CacheElem> cache;
    /// The PIOL object
    std::shared_ptr<ExSeisPIOL> piol;

  public:
    /*! Initialise the cache.
     * @param[in] piol_ The PIOL object
     */
    Cache(std::shared_ptr<ExSeisPIOL> piol_) : piol(piol_) {}

    /*! Get a given cache of parameters or traces. Perform I/O and cache the
     *  result if not already done so.
     *  @param[in] rule The rule to use for the parameters.
     *  @param[in] desc A deque of unique pointers to file descriptors.
     *  @param[in] cPrm if True, get the parameters.
     *  @param[in] cTrc if True, get the traces.
     *  @return Return a block with the traces and/or parameters.
     */
    std::shared_ptr<TraceBlock> getCache(
      std::shared_ptr<File::Rule> rule, FileDeque& desc, bool cPrm, bool cTrc);

    /*! Get a given cache of parameters. Perform I/O and cache the result if not
     *  already done so.
     *  @param[in] rule The rule to use for the parameters.
     *  @param[in] desc A deque of unique pointers to file descriptors.
     *  @return Return a block with the parameters.
     */
    std::shared_ptr<TraceBlock> cachePrm(
      std::shared_ptr<File::Rule> rule, FileDeque& desc)
    {
        return getCache(rule, desc, true, false);
    }

    /*! Get a given cache of traces. Perform I/O and cache the result if not
     *  already done so.
     *  @param[in] desc A deque of unique pointers to file descriptors.
     *  @return Return a block with the traces.
     */
    std::shared_ptr<TraceBlock> cacheTrc(FileDeque& desc)
    {
        return getCache(nullptr, desc, false, true);
    }

    /*! Check if any of elements have cached parameters
     *  @param[in] desc A deque of unique pointers to file descriptors.
     *  @return Return true if the parameters are cached.
     */
    bool checkPrm(FileDeque& desc)
    {
        auto it = std::find_if(
          cache.begin(), cache.end(), [desc](const CacheElem& elem) -> bool {
              return elem.checkPrm(desc);
          });
        return it != cache.end();
    }

    /*! Check if any of elements have cached traces
     *  @param[in] desc A deque of unique pointers to file descriptors.
     *  @return Return true if the traces are cached.
     */
    bool checkTrc(FileDeque& desc)
    {
        auto it = std::find_if(
          cache.begin(), cache.end(), [desc](const CacheElem& elem) -> bool {
              return elem.checkTrc(desc);
          });
        return it != cache.end();
    }

    /*! Erase the cache corresponding to the descriptor
     *  @param[in] desc A deque of unique pointers to file descriptors.
     */
    void flush(FileDeque& desc)
    {
        auto it = std::find_if(
          cache.begin(), cache.end(),
          [desc](const CacheElem& elem) -> bool { return elem.desc == desc; });
        cache.erase(it);
    }

    /*! Get a subset of parameters from a cache.
     *  @param[in] desc A deque of unique pointers to file descriptors.
     *  @param[in] offset The offset in the file subset.
     *  @param[in] sz The number of traces from the file subset.
     *  @param[out] prm The parameter structure to fill.
     *  @return Return the output trace locations
     */
    std::vector<size_t> getOutputTrace(
      FileDeque& desc, const size_t offset, const size_t sz, File::Param* prm);
};

}  // namespace PIOL

#endif
