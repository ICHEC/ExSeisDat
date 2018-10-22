////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_CACHEELEM_HH
#define EXSEISDAT_FLOW_CACHEELEM_HH

#include "exseisdat/flow/FileDesc.hh"
#include "exseisdat/flow/TraceBlock.hh"

#include <deque>
#include <memory>
#include <vector>

namespace exseis {
namespace flow {

/*! A structure to store cache elements of traces and parameters corresponding
 *  to a collection of files
 */
struct CacheElem {
    /// Typedef for passing in a list of FileDesc objects.
    typedef std::deque<std::shared_ptr<FileDesc>> FileDeque;

    /// A deque of unique pointers to file descriptors
    FileDeque desc;

    /// The cached data
    std::shared_ptr<TraceBlock> block = std::make_shared<TraceBlock>();

    /*! Construct the cache element with the given parameter structure.
     *  @param[in] desc A deque of unique pointers to file descriptors
     *  @param[in,out] prm A unique_ptr to the parameter structure. The cache
     *                element takes ownership.
     */
    CacheElem(
      FileDeque& desc, std::unique_ptr<exseis::piol::Trace_metadata> prm) :
        desc(desc)
    {
        block->prm = std::move(prm);
    }

    /*! Construct the cache element with the given parameter structure.
     *  @param[in] desc A deque of unique pointers to file descriptors
     *  @param[in] trc A vector of traces for caching.
     *  @param[in,out] prm A unique_ptr to the parameter structure. The cache
     *                element takes ownership if it exists.
     */
    CacheElem(
      FileDeque& desc,
      std::vector<exseis::utils::Trace_value>& trc,
      std::unique_ptr<exseis::piol::Trace_metadata> prm = nullptr) :
        desc(desc)
    {
        block->trc = std::move(trc);
        block->prm = std::move(prm);
    }

    /*! Check if the given element has cached parameters
     *  @param[in] test_files A deque of unique pointers to file descriptors
     *  @return Return true if the parameters are cached.
     */
    bool check_prm(const FileDeque& test_files) const
    {
        return test_files == desc && !block && block->prm;
    }

    /*! Check if the given element has cached traces
     *  @param[in] test_files A deque of unique pointers to file descriptors
     *  @return Return true if the traces are cached.
     */
    bool check_trc(const FileDeque& test_files) const
    {
        return test_files == desc && !block && !block->trc.empty();
    }
};

}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_CACHEELEM_HH
