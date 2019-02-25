////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////

#include "exseisdat/flow/RadonGatherState.hh"

#include "exseisdat/piol/ReadSEGYModel.hh"

using namespace exseis::piol;

namespace exseis {
namespace flow {

void RadonGatherState::make_state(
    const std::vector<size_t>& offset,
    const utils::Distributed_vector<Gather_info>& gather)
{
    // TODO: DON'T USE MAGIC NAME
    ReadSEGYModel vm(piol, vmname);
    v_ns                           = vm.read_ns();
    velocity_model_sample_interval = vm.read_sample_interval();

    vtrc = vm.read_model(offset.size(), offset.data(), gather);

    il.resize(offset.size());
    xl.resize(offset.size());

    for (size_t i = 0; i < offset.size(); i++) {
        auto gval = gather[offset[i]];
        il[i]     = gval.in_line;
        xl[i]     = gval.crossline;
    }
}

}  // namespace flow
}  // namespace exseis
