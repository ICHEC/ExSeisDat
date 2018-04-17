////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_OP_HH
#define EXSEISDAT_FLOW_OP_HH

#include "ExSeisDat/Flow/OpParent.hh"

#include "ExSeisDat/Flow/GatherState.hh"
#include "ExSeisDat/Flow/OpOpt.hh"
#include "ExSeisDat/PIOL/Rule.hh"

#include <memory>

namespace exseis {
namespace Flow {

/*! Template for creating a structure for a particular operation type.
 */
template<typename T>
struct Op : public OpParent {
    /// The particular std::function object for the operaton
    T func;

    /*! Construct.
     *  @param[in] opt_ Operation options.
     *  @param[in] rule_ Rules parameter rules for the operation
     *  @param[in] state_ Gather state object if applicable.
     *  @param[in] func_ The particular std::function implementation.
     */
    Op(
      OpOpt& opt_,
      std::shared_ptr<exseis::PIOL::Rule> rule_,
      std::shared_ptr<GatherState> state_,
      T func_) :
        OpParent(opt_, rule_, state_),
        func(func_)
    {
    }
};

}  // namespace Flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_OP_HH
