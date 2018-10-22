////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_GATHERSTATE_HH
#define EXSEISDAT_FLOW_GATHERSTATE_HH

#include "exseisdat/piol/operations/gather.hh"
#include "exseisdat/utils/Distributed_vector.hh"

#include <cstddef>
#include <vector>

namespace exseis {
namespace flow {

/*! A parent class to allow gather operations to maintain a state.
 */
struct GatherState {
    /// Virtual destructor
    virtual ~GatherState() = default;

    /*! A virtual function which can be overridden to create the
     *  gather-operation state.
     *  @param[in] offset A list of gather-numbers to be processed by the local
     *             process.
     *  @param[in] gather The global array of gathers.
     */
    virtual void make_state(
      const std::vector<size_t>& offset,
      const exseis::utils::Distributed_vector<exseis::piol::Gather_info>&
        gather) = 0;
};

}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_GATHERSTATE_HH
