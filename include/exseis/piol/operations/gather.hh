////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief
/// @todo Note, these functions are intentionally specific because we need
///       further use cases to generalise the functionality.
////////////////////////////////////////////////////////////////////////////////

#ifndef EXSEIS_PIOL_OPERATIONS_GATHER_HH
#define EXSEIS_PIOL_OPERATIONS_GATHER_HH

#include "exseis/piol/file/Input_file.hh"
#include "exseis/utils/distributed_vector/Distributed_vector.hh"
#include "exseis/utils/types/typedefs.hh"

namespace exseis {
inline namespace piol {
inline namespace operations {

/// A struct containing information about a seismic gather.
struct Gather_info {
    /// The number of traces in the gather.
    size_t num_traces;

    /// The inline coordinate of the gather (in_line because of reserved word).
    Integer in_line;

    /// The crossline coordinate of the gather.
    Integer crossline;
};


/// @brief Find the inline/crossline for each il/xl gather and the number of
///        traces per gather using the parameters from the file provided.
///
/// @param[in] communicator  The communicator to decompose the problem over
/// @param[in] file The file which has il/xl gathers.
///
/// @return Return an 'array' of tuples. Each tuple corresponds to each gather.
///         Tuple elements: 1) Number of traces in the gather, 2) inline, 3)
///         crossline.
///
Distributed_vector<Gather_info> get_il_xl_gathers(
    const Communicator& communicator, const Input_file& file);

}  // namespace operations
}  // namespace piol
}  // namespace exseis

#endif  // EXSEIS_PIOL_OPERATIONS_GATHER_HH
