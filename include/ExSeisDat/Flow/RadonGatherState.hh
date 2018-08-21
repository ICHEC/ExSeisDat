////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_RADONGATHERSTATE_HH
#define EXSEISDAT_FLOW_RADONGATHERSTATE_HH

#include "ExSeisDat/Flow/GatherState.hh"

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/operations/gather.hh"
#include "ExSeisDat/utils/Distributed_vector.hh"
#include "ExSeisDat/utils/typedefs.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace exseis {
namespace Flow {

using namespace exseis::utils::typedefs;

/*! The radon state structure.
 */
struct RadonGatherState : public GatherState {
    /// The piol object.
    std::shared_ptr<exseis::PIOL::ExSeisPIOL> piol;

    /// The name of the Velocity Model (VM) file.
    std::string vmname;

    /// Trace data read from the VM file.
    std::vector<exseis::utils::Trace_value> vtrc;

    /// A list of inlines corresponding to the VM data read.
    std::vector<exseis::utils::Integer> il;

    /// A list of crosslines corresponding to the VM data read.
    std::vector<exseis::utils::Integer> xl;

    /// The number of samples per trace for the VM.
    size_t vNs;

    /// The binning factor to be used.
    size_t vBin;

    /// The number of traces per gather in the angle output.
    size_t oGSz;

    /// The increment between samples in the VM file.
    exseis::utils::Floating_point vInc;

    /// The increment between samples in the output file (radians).
    exseis::utils::Floating_point oInc;

    /*! Constructor for the radon state.
     * @param[in] piol_ The piol object.
     * @param[in] vmname_ The VM file.
     * @param[in] vBin_ The velocity model bin parameter
     * @param[in] oGSz_  The number of traces in the angle output.
     * @param[in] oInc_ The number of increments.
     */
    RadonGatherState(
      std::shared_ptr<exseis::PIOL::ExSeisPIOL> piol_,
      std::string vmname_,
      const size_t vBin_,
      const size_t oGSz_,
      const exseis::utils::Floating_point oInc_) :
        piol(piol_),
        vmname(vmname_),
        vNs(0),
        vBin(vBin_),
        oGSz(oGSz_),
        vInc(0),
        oInc(oInc_)
    {
    }

    void makeState(
      const std::vector<size_t>& offset,
      const exseis::utils::Distributed_vector<exseis::PIOL::Gather_info>&
        gather) override;
};

}  // namespace Flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_RADONGATHERSTATE_HH
