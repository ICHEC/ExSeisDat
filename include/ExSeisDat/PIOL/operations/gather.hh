////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @todo Note, these functions are intentionally specific because we need
///       further use cases to generalise the functionality.
////////////////////////////////////////////////////////////////////////////////

#ifndef EXSEISDAT_PIOL_OPERATIONS_GATHER_HH
#define EXSEISDAT_PIOL_OPERATIONS_GATHER_HH

#include "ExSeisDat/PIOL/Distributed_vector.hh"
#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/ReadInterface.hh"

namespace PIOL {

/// A struct containing information about a seismic gather.
struct Gather_info {
    /// The number of traces in the gather.
    size_t num_traces;

    /// The inline coordinate of the gather (inline_ because of reserved word).
    llint inline_;

    /// The crossline coordinate of the gather.
    llint crossline;
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
Distributed_vector<Gather_info> getIlXlGathers(
  ExSeisPIOL* piol, ReadInterface* file);

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_OPERATIONS_GATHER_HH
