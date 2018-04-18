////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @todo Note, these functions are intentionally specific because we need
///       further use cases to generalise the functionality.
////////////////////////////////////////////////////////////////////////////////

#ifndef EXSEISDAT_PIOL_OPERATIONS_GATHER_HH
#define EXSEISDAT_PIOL_OPERATIONS_GATHER_HH

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/ReadInterface.hh"
#include "ExSeisDat/utils/Distributed_vector.hh"
#include "ExSeisDat/utils/typedefs.h"

namespace exseis {
namespace PIOL {

using namespace exseis::utils::typedefs;

/// A struct containing information about a seismic gather.
struct Gather_info {
    /// The number of traces in the gather.
    size_t num_traces;

    /// The inline coordinate of the gather (inline_ because of reserved word).
    exseis::utils::Integer inline_;

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
exseis::utils::Distributed_vector<Gather_info> getIlXlGathers(
  ExSeisPIOL* piol, ReadInterface* file);

}  // namespace PIOL
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_OPERATIONS_GATHER_HH
