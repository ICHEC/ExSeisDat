////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_RADONGATHERSTATE_HH
#define EXSEISDAT_FLOW_RADONGATHERSTATE_HH

#include "ExSeisDat/Flow/GatherState.hh"

#include "ExSeisDat/PIOL/Distributed_vector.hh"
#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/typedefs.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace PIOL {

/*! The radon state structure.
 */
struct RadonGatherState : public GatherState {
    /// The piol object.
    std::shared_ptr<ExSeisPIOL> piol;

    /// The name of the Velocity Model (VM) file.
    std::string vmname;

    /// Trace data read from the VM file.
    std::vector<trace_t> vtrc;

    /// A list of inlines corresponding to the VM data read.
    std::vector<llint> il;

    /// A list of crosslines corresponding to the VM data read.
    std::vector<llint> xl;

    /// The number of samples per trace for the VM.
    size_t vNs;

    /// The binning factor to be used.
    size_t vBin;

    /// The number of traces per gather in the angle output.
    size_t oGSz;

    /// The increment between samples in the VM file.
    geom_t vInc;

    /// The increment between samples in the output file (radians).
    geom_t oInc;

    /*! Constructor for the radon state.
     * @param[in] piol_ The piol object.
     * @param[in] vmname_ The VM file.
     * @param[in] vBin_ The velocity model bin parameter
     * @param[in] oGSz_  The number of traces in the angle output.
     * @param[in] oInc_ The number of increments.
     */
    RadonGatherState(
      std::shared_ptr<ExSeisPIOL> piol_,
      std::string vmname_,
      const size_t vBin_,
      const size_t oGSz_,
      const geom_t oInc_) :
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
      const Distributed_vector<std::tuple<size_t, llint, llint>>& gather);
};

}  // namespace PIOL

#endif  // EXSEISDAT_FLOW_RADONGATHERSTATE_HH
