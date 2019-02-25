////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_CACHE_HH
#define EXSEISDAT_FLOW_CACHE_HH

#include "exseisdat/flow/CacheElem.hh"
#include "exseisdat/flow/FileDesc.hh"
#include "exseisdat/flow/TraceBlock.hh"

#include <algorithm>
#include <deque>

namespace exseis {
namespace flow {

/*! The class which holds all cache elements.
 */
class Cache {
  public:
    /// Typedef for passing in a list of FileDesc objects.
    typedef std::deque<std::shared_ptr<FileDesc>> FileDeque;

  private:
    /// A vector of cache elements
    std::vector<CacheElem> m_cache;

    /// The PIOL object
    std::shared_ptr<exseis::piol::ExSeisPIOL> m_piol;

  public:
    /*! Initialise the cache.
     * @param[in] piol The PIOL object
     */
    Cache(std::shared_ptr<exseis::piol::ExSeisPIOL> piol) : m_piol(piol) {}

    /*! Get a given cache of parameters or traces. Perform I/O and cache the
     *  result if not already done so.
     *  @param[in] rule The rule to use for the parameters.
     *  @param[in] desc A deque of unique pointers to file descriptors.
     *  @param[in] c_prm if True, get the parameters.
     *  @param[in] c_trc if True, get the traces.
     *  @return Return a block with the traces and/or parameters.
     */
    std::shared_ptr<TraceBlock> get_cache(
        std::shared_ptr<exseis::piol::Rule> rule,
        FileDeque& desc,
        bool c_prm,
        bool c_trc);

    /*! Get a given cache of parameters. Perform I/O and cache the result if not
     *  already done so.
     *  @param[in] rule The rule to use for the parameters.
     *  @param[in] desc A deque of unique pointers to file descriptors.
     *  @return Return a block with the parameters.
     */
    std::shared_ptr<TraceBlock> cache_prm(
        std::shared_ptr<exseis::piol::Rule> rule, FileDeque& desc)
    {
        return get_cache(rule, desc, true, false);
    }

    /*! Get a given cache of traces. Perform I/O and cache the result if not
     *  already done so.
     *  @param[in] desc A deque of unique pointers to file descriptors.
     *  @return Return a block with the traces.
     */
    std::shared_ptr<TraceBlock> cache_trc(FileDeque& desc)
    {
        return get_cache(nullptr, desc, false, true);
    }

    /*! Check if any of elements have cached parameters
     *  @param[in] desc A deque of unique pointers to file descriptors.
     *  @return Return true if the parameters are cached.
     */
    bool check_prm(FileDeque& desc)
    {
        auto it = std::find_if(
            m_cache.begin(), m_cache.end(),
            [desc](const CacheElem& elem) -> bool {
                return elem.check_prm(desc);
            });
        return it != m_cache.end();
    }

    /*! Check if any of elements have cached traces
     *  @param[in] desc A deque of unique pointers to file descriptors.
     *  @return Return true if the traces are cached.
     */
    bool check_trc(FileDeque& desc)
    {
        auto it = std::find_if(
            m_cache.begin(), m_cache.end(),
            [desc](const CacheElem& elem) -> bool {
                return elem.check_trc(desc);
            });
        return it != m_cache.end();
    }

    /*! Erase the cache corresponding to the descriptor
     *  @param[in] desc A deque of unique pointers to file descriptors.
     */
    void flush(FileDeque& desc)
    {
        auto it = std::find_if(
            m_cache.begin(), m_cache.end(),
            [desc](const CacheElem& elem) -> bool {
                return elem.desc == desc;
            });
        m_cache.erase(it);
    }

    /*! Get a subset of parameters from a cache.
     *  @param[in] desc A deque of unique pointers to file descriptors.
     *  @param[in] offset The offset in the file subset.
     *  @param[in] sz The number of traces from the file subset.
     *  @param[out] prm The parameter structure to fill.
     *  @return Return the output trace locations
     */
    std::vector<size_t> get_output_trace(
        FileDeque& desc,
        size_t offset,
        size_t sz,
        exseis::piol::Trace_metadata& prm);
};

}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_CACHE_HH
