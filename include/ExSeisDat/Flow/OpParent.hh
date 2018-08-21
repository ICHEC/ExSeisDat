////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_OPPARENT_HH
#define EXSEISDAT_FLOW_OPPARENT_HH

#include "ExSeisDat/Flow/GatherState.hh"
#include "ExSeisDat/Flow/OpOpt.hh"
#include "ExSeisDat/PIOL/Rule.hh"

#include <memory>

namespace exseis {
namespace Flow {

/*! Operations parents. Specific classes of operations inherit from this parent
 */
struct OpParent {
    /// Operation options.
    OpOpt opt;

    /// Relevant parameter rules for the operation.
    std::shared_ptr<exseis::PIOL::Rule> rule;

    /// Gather state if applicable.
    std::shared_ptr<GatherState> state;

    /*! Construct.
     *  @param[in] opt_ Operation options.
     *  @param[in] rule_ Rules parameter rules for the operation
     *  @param[in] state_ Gather state object if applicable.
     */
    OpParent(
      OpOpt& opt_,
      std::shared_ptr<exseis::PIOL::Rule> rule_,
      std::shared_ptr<GatherState> state_) :
        opt(opt_),
        rule(rule_),
        state(state_)
    {
    }

    /*! Virtual destructor for unique_ptr polymorphism.
     */
    virtual ~OpParent(void) {}
};

}  // namespace Flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_OPPARENT_HH
