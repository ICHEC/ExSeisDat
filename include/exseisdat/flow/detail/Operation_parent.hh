////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_DETAIL_OPERATION_PARENT_HH
#define EXSEISDAT_FLOW_DETAIL_OPERATION_PARENT_HH

#include "exseisdat/flow/detail/Gather_state.hh"
#include "exseisdat/flow/detail/Operation_options.hh"
#include "exseisdat/piol/metadata/rules/Rule.hh"

#include <memory>

namespace exseis {
namespace flow {
namespace detail {

/*! Operations parents. Specific classes of operations inherit from this parent
 */
struct Operation_parent {
    /// Operation options.
    Operation_options opt;

    /// Relevant parameter rules for the operation.
    std::shared_ptr<exseis::piol::Rule> rule;

    /// Gather state if applicable.
    std::shared_ptr<Gather_state> state;

    /*! Construct.
     *  @param[in] opt Operation options.
     *  @param[in] rule Rules parameter rules for the operation
     *  @param[in] state Gather state object if applicable.
     */
    Operation_parent(
        Operation_options& opt,
        std::shared_ptr<exseis::piol::Rule> rule,
        std::shared_ptr<Gather_state> state) :
        opt(opt), rule(rule), state(state)
    {
    }

    /*! Virtual destructor for unique_ptr polymorphism.
     */
    virtual ~Operation_parent() = default;
};

}  // namespace detail
}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_DETAIL_OPERATION_PARENT_HH
