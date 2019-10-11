////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_DETAIL_CACHE_ELEM_HH
#define EXSEISDAT_FLOW_DETAIL_CACHE_ELEM_HH

#include "exseisdat/flow/detail/File_descriptor.hh"
#include "exseisdat/flow/detail/Trace_block.hh"

#include <deque>
#include <memory>
#include <vector>

namespace exseis {
namespace flow {
namespace detail {

/*! A structure to store cache elements of traces and parameters corresponding
 *  to a collection of files
 */
struct Cache_elem {
    /// Typedef for passing in a list of FileDesc objects.
    typedef std::deque<std::shared_ptr<File_descriptor>> FileDeque;

    /// A deque of unique pointers to file descriptors
    FileDeque desc;

    /// The cached data
    std::shared_ptr<Trace_block> block = std::make_shared<Trace_block>();

    /*! Construct the cache element with the given parameter structure.
     *  @param[in] desc A deque of unique pointers to file descriptors
     *  @param[in,out] prm A unique_ptr to the parameter structure. The cache
     *                element takes ownership.
     */
    Cache_elem(
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
    Cache_elem(
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

}  // namespace detail
}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_DETAIL_CACHE_ELEM_HH
