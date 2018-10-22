////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @todo Note, these functions are intentionally specific because we need
///       further use cases to generalise the functionality.
////////////////////////////////////////////////////////////////////////////////

#ifndef EXSEISDAT_PIOL_OPERATIONS_GATHER_HH
#define EXSEISDAT_PIOL_OPERATIONS_GATHER_HH

#include "exseisdat/piol/ExSeisPIOL.hh"
#include "exseisdat/piol/ReadInterface.hh"
#include "exseisdat/utils/Distributed_vector.hh"
#include "exseisdat/utils/typedefs.hh"

namespace exseis {
namespace piol {

using namespace exseis::utils::typedefs;

/// A struct containing information about a seismic gather.
struct Gather_info {
    /// The number of traces in the gather.
    size_t num_traces;

    /// The inline coordinate of the gather (in_line because of reserved word).
    exseis::utils::Integer in_line;

    /// The crossline coordinate of the gather.
    exseis::utils::Integer crossline;
};


/// Find the inline/crossline for each il/xl gather and the number of traces per
/// gather using the parameters from the file provided.
///
/// @param[in] piol The piol object.
/// @param[in] file The file which has il/xl gathers.
///
/// @return Return an 'array' of tuples. Each tuple corresponds to each gather.
///         Tuple elements: 1) Number of traces in the gather, 2) inline, 3)
///         crossline.
///
exseis::utils::Distributed_vector<Gather_info> get_il_xl_gathers(
  ExSeisPIOL* piol, ReadInterface* file);

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_OPERATIONS_GATHER_HH
