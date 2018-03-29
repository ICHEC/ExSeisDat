////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details Implementation for \c ReadModel
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/ReadModel.hh"

#include "ExSeisDat/PIOL/ReadSEGYModel.hh"
#include "ExSeisDat/PIOL/data/datampiio.hh"
#include "ExSeisDat/PIOL/object/objsegy.hh"

namespace PIOL {

ReadModel::ReadModel(std::shared_ptr<ExSeisPIOL> piol, const std::string name) :
    ReadDirect(
      piol, name, Data::MPIIO::Opt(), Obj::SEGY::Opt(), ReadSEGYModel::Opt())
{
}

std::vector<trace_t> ReadModel::readModel(
  size_t gOffset, size_t numGather, Uniray<size_t, llint, llint>& gather)
{
    return std::dynamic_pointer_cast<Model3dInterface>(file)->readModel(
      gOffset, numGather, gather);
}

}  // namespace PIOL
