////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/Flow/RadonGatherState.hh"

#include "ExSeisDat/PIOL/ReadSEGYModel.hh"
#include "ExSeisDat/PIOL/makeFile.hh"

namespace PIOL {

void RadonGatherState::makeState(
  const std::vector<size_t>& offset, const Uniray<size_t, llint, llint>& gather)
{
    // TODO: DON'T USE MAGIC NAME
    std::unique_ptr<ReadSEGYModel> vm = makeFile<ReadSEGYModel>(piol, vmname);
    vNs                               = vm->readNs();
    vInc                              = vm->readInc();

    vtrc = vm->readModel(offset.size(), offset.data(), gather);

    il.resize(offset.size());
    xl.resize(offset.size());

    for (size_t i = 0; i < offset.size(); i++) {
        auto gval = gather[offset[i]];
        il[i]     = std::get<1>(gval);
        xl[i]     = std::get<2>(gval);
    }
}

}  // namespace PIOL
