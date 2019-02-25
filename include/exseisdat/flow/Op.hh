////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_OP_HH
#define EXSEISDAT_FLOW_OP_HH

#include "exseisdat/flow/OpParent.hh"

#include "exseisdat/flow/GatherState.hh"
#include "exseisdat/flow/OpOpt.hh"
#include "exseisdat/piol/Rule.hh"

#include <memory>

namespace exseis {
namespace flow {

/*! Template for creating a structure for a particular operation type.
 */
template<typename T>
struct Op : public OpParent {
    /// The particular std::function object for the operaton
    T func;

    /*! Construct.
     *  @param[in] opt Operation options.
     *  @param[in] rule Rules parameter rules for the operation
     *  @param[in] state Gather state object if applicable.
     *  @param[in] func The particular std::function implementation.
     */
    Op(OpOpt& opt,
       std::shared_ptr<exseis::piol::Rule> rule,
       std::shared_ptr<GatherState> state,
       T func) :
        OpParent(opt, rule, state),
        func(func)
    {
    }
};

}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_OP_HH
