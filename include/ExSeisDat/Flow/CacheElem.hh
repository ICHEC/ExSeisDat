////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_CACHEELEM_HH
#define EXSEISDAT_FLOW_CACHEELEM_HH

#include "ExSeisDat/Flow/FileDesc.hh"
#include "ExSeisDat/Flow/TraceBlock.hh"

#include <deque>
#include <memory>
#include <vector>

namespace exseis {
namespace Flow {

/*! A structure to store cache elements of traces and parameters corresponding
 *  to a collection of files
 */
struct CacheElem {
    /// Typedef for passing in a list of FileDesc objects.
    typedef std::deque<std::shared_ptr<FileDesc>> FileDeque;

    /// A deque of unique pointers to file descriptors
    FileDeque desc;

    /// The cached data
    std::shared_ptr<TraceBlock> block;

    /*! Construct the cache element with the given parameter structure.
     *  @param[in] desc_ A deque of unique pointers to file descriptors
     *  @param[inout] prm_ A unique_ptr to the parameter structure. The cache
     *                element takes ownership.
     */
    CacheElem(FileDeque& desc_, std::unique_ptr<exseis::PIOL::Param> prm_)
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
      std::vector<exseis::utils::Trace_value>& trc_,
      std::unique_ptr<exseis::PIOL::Param> prm_ = nullptr)
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
        return desc == desc_ && !block && !block->trc.empty();
    }
};

}  // namespace Flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_CACHEELEM_HH
