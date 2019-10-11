////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_DETAIL_RADON_GATHER_STATE_HH
#define EXSEISDAT_FLOW_DETAIL_RADON_GATHER_STATE_HH

#include "exseisdat/flow/detail/Gather_state.hh"

#include "exseisdat/piol/configuration/ExSeisPIOL.hh"
#include "exseisdat/piol/operations/gather.hh"
#include "exseisdat/utils/distributed_vector/Distributed_vector.hh"
#include "exseisdat/utils/types/typedefs.hh"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace exseis {
namespace flow {
namespace detail {

using namespace exseis::utils::types;

/*! The radon state structure.
 */
struct Radon_gather_state : public Gather_state {
    /// The piol object.
    std::shared_ptr<exseis::piol::ExSeisPIOL> piol;

    /// The name of the Velocity Model (VM) file.
    std::string vmname;

    /// Trace data read from the VM file.
    std::vector<exseis::utils::Trace_value> vtrc;

    /// A list of inlines corresponding to the VM data read.
    std::vector<exseis::utils::Integer> il;

    /// A list of crosslines corresponding to the VM data read.
    std::vector<exseis::utils::Integer> xl;

    /// The number of samples per trace for the VM.
    size_t v_ns;

    /// The binning factor to be used.
    size_t v_bin;

    /// The number of traces per gather in the angle output.
    size_t output_traces_per_gather;

    /// The interval between samples in the VM file.
    exseis::utils::Floating_point velocity_model_sample_interval;

    /// The interval between samples in the output file (radians).
    exseis::utils::Floating_point output_sample_interval;

    /*! Constructor for the radon state.
     * @param[in] piol The piol object.
     * @param[in] vmname The VM file.
     * @param[in] v_bin The velocity model bin parameter
     * @param[in] output_traces_per_gather  The number of traces in the angle output.
     * @param[in] output_sample_interval The number of increments.
     */
    Radon_gather_state(
        std::shared_ptr<exseis::piol::ExSeisPIOL> piol,
        std::string vmname,
        const size_t v_bin,
        const size_t output_traces_per_gather,
        const exseis::utils::Floating_point output_sample_interval) :
        piol(piol),
        vmname(vmname),
        v_ns(0),
        v_bin(v_bin),
        output_traces_per_gather(output_traces_per_gather),
        velocity_model_sample_interval(0),
        output_sample_interval(output_sample_interval)
    {
    }

    void make_state(
        const std::vector<size_t>& offset,
        const exseis::utils::Distributed_vector<exseis::piol::Gather_info>&
            gather) override;
};

}  // namespace detail
}  // namespace flow
}  // namespace exseis

#endif  // EXSEISDAT_FLOW_DETAIL_RADON_GATHER_STATE_HH
