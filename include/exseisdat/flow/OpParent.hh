////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_OPPARENT_HH
#define EXSEISDAT_FLOW_OPPARENT_HH

#include "exseisdat/flow/GatherState.hh"
#include "exseisdat/flow/OpOpt.hh"
#include "exseisdat/piol/Rule.hh"

#include <memory>

namespace exseis {
namespace flow {

/*! Operations parents. Specific classes of operations inherit from this parent
 */
struct OpParent {
    /// Operation options.
    OpOpt opt;

    /// Relevant parameter rules for the operation.
    std::shared_ptr<exseis::piol::Rule> rule;

    /// Gather state if applicable.
    std::shared_ptr<GatherState> state;

    /*! Construct.
     *  @param[in] opt Operation options.
     *  @param[in] rule Rules parameter rules for the operation
     *  @param[in] state Gather state object if applicable.
     */
    OpParent(
        OpOpt& opt,
        std::shared_ptr<exseis::piol::Rule> rule,
        std::shared_ptr<GatherState> state) :
        opt(opt),
        rule(rule),
        state(state)
    {
    }

    /*! Virtual destructor for unique_ptr polymorphism.
     */
    virtual ~OpParent(void) = default;
};

}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_OPPARENT_HH
