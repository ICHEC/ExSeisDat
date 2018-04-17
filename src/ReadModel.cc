////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details Implementation for \c ReadModel
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/ReadModel.hh"

#include "ExSeisDat/PIOL/DataMPIIO.hh"
#include "ExSeisDat/PIOL/ObjectSEGY.hh"
#include "ExSeisDat/PIOL/ReadSEGYModel.hh"

namespace exseis {
namespace PIOL {

ReadModel::ReadModel(std::shared_ptr<ExSeisPIOL> piol, const std::string name) :
    ReadDirect(
      piol, name, DataMPIIO::Opt(), ObjectSEGY::Opt(), ReadSEGYModel::Opt())
{
}

std::vector<trace_t> ReadModel::readModel(
  size_t gOffset,
  size_t numGather,
  utils::Distributed_vector<Gather_info>& gather)
{
    return std::dynamic_pointer_cast<Model3dInterface>(file)->readModel(
      gOffset, numGather, gather);
}

}  // namespace PIOL
}  // namespace exseis
