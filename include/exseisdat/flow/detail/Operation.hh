////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_DETAIL_OPERATION_HH
#define EXSEISDAT_FLOW_DETAIL_OPERATION_HH

#include "exseisdat/flow/detail/Operation_parent.hh"

#include "exseisdat/flow/detail/Gather_state.hh"
#include "exseisdat/flow/detail/Operation_options.hh"
#include "exseisdat/piol/metadata/rules/Rule.hh"

#include <memory>

namespace exseis {
namespace flow {
namespace detail {

/*! Template for creating a structure for a particular operation type.
 */
template<typename T>
struct Operation : public Operation_parent {
    /// The particular std::function object for the operaton
    T func;

    /*! Construct.
     *  @param[in] opt Operation options.
     *  @param[in] rule Rules parameter rules for the operation
     *  @param[in] state Gather state object if applicable.
     *  @param[in] func The particular std::function implementation.
     */
    Operation(
        Operation_options& opt,
        std::shared_ptr<exseis::piol::Rule> rule,
        std::shared_ptr<Gather_state> state,
        T func) :
        Operation_parent(opt, rule, state), func(func)
    {
    }
};

}  // namespace detail
}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_DETAIL_OPERATION_HH
