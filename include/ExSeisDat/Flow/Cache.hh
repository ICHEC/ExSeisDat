////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_CACHE_HH
#define EXSEISDAT_FLOW_CACHE_HH

#include "ExSeisDat/Flow/CacheElem.hh"
#include "ExSeisDat/Flow/FileDesc.hh"
#include "ExSeisDat/Flow/TraceBlock.hh"

#include <algorithm>
#include <deque>

namespace exseis {
namespace Flow {

/*! The class which holds all cache elements.
 */
class Cache {
  public:
    /// Typedef for passing in a list of FileDesc objects.
    typedef std::deque<std::shared_ptr<FileDesc>> FileDeque;

  private:
    /// A vector of cache elements
    std::vector<CacheElem> cache;

    /// The PIOL object
    std::shared_ptr<exseis::PIOL::ExSeisPIOL> piol;

  public:
    /*! Initialise the cache.
     * @param[in] piol_ The PIOL object
     */
    Cache(std::shared_ptr<exseis::PIOL::ExSeisPIOL> piol_) : piol(piol_) {}

    /*! Get a given cache of parameters or traces. Perform I/O and cache the
     *  result if not already done so.
     *  @param[in] rule The rule to use for the parameters.
     *  @param[in] desc A deque of unique pointers to file descriptors.
     *  @param[in] cPrm if True, get the parameters.
     *  @param[in] cTrc if True, get the traces.
     *  @return Return a block with the traces and/or parameters.
     */
    std::shared_ptr<TraceBlock> getCache(
      std::shared_ptr<exseis::PIOL::Rule> rule,
      FileDeque& desc,
      bool cPrm,
      bool cTrc);

    /*! Get a given cache of parameters. Perform I/O and cache the result if not
     *  already done so.
     *  @param[in] rule The rule to use for the parameters.
     *  @param[in] desc A deque of unique pointers to file descriptors.
     *  @return Return a block with the parameters.
     */
    std::shared_ptr<TraceBlock> cachePrm(
      std::shared_ptr<exseis::PIOL::Rule> rule, FileDeque& desc)
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
      FileDeque& desc, size_t offset, size_t sz, exseis::PIOL::Param* prm);
};

}  // namespace Flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_CACHE_HH
