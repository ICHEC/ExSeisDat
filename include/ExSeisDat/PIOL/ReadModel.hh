////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @details The \c ReadModel class
/// @todo document me
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_READMODEL_HH
#define EXSEISDAT_PIOL_READMODEL_HH

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/ReadDirect.hh"
#include "ExSeisDat/PIOL/operations/gather.hh"

#include <memory>
#include <vector>

namespace exseis {
namespace PIOL {

/// @todo document me
class ReadModel : public ReadDirect {
  public:
    /// @param[in] piol_ This PIOL ptr is not modified but is used to
    ///                  instantiate another shared_ptr.
    /// @param[in] name_ The name of the file associated with the instantiation.
    ReadModel(std::shared_ptr<ExSeisPIOL> piol_, const std::string name_);

    /// @todo DOCUMENT brief and return type
    /// @param[in] gOffset   DOCUMENT ME
    /// @param[in] numGather DOCUMENT ME
    /// @param[in] gather    DOCUMENT ME
    /// @return DOCUMENT ME
    std::vector<trace_t> virtual readModel(
      size_t gOffset,
      size_t numGather,
      exseis::utils::Distributed_vector<Gather_info>& gather);
};

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_READMODEL_HH
