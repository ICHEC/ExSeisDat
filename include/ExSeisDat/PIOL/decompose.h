////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c decompose functions decompose a range over a number of
///        processes.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_DECOMPOSE_H
#define EXSEISDAT_PIOL_DECOMPOSE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// The \c Contiguous_decomposition class represents a section of a range on the given
/// rank which has been distributed over a number of ranks.
struct PIOL_Contiguous_decomposition {
    /// The offset into the global range for the given rank.
    size_t global_offset;

    /// The local size of the section of the range on the local process
    size_t local_size;
};

/// @brief Perform a 1d decomposition of the interval [0,range_size-1] into
///        `num_ranks` pieces so it is optimally spread across each `rank`.
///
/// @param[in] range_size The total size of the 1d domain.
/// @param[in] num_ranks  The number of ranks to perform the decomposition
///                       over.
/// @param[in] rank       The rank of the local process.
/// @return The section of the range decomposed onto rank `rank`.
///
/// @pre num_ranks > 0
/// @pre rank < num_ranks
///
struct PIOL_Contiguous_decomposition PIOL_block_decompose(
  size_t range_size, size_t num_ranks, size_t rank);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus


#ifdef __cplusplus

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/ReadInterface.hh"

namespace PIOL {

/// @copydoc PIOL_Contiguous_decomposition
using Contiguous_decomposition = PIOL_Contiguous_decomposition;


/// @copydoc PIOL_block_decompose
inline Contiguous_decomposition block_decompose(
  size_t range_size, size_t num_ranks, size_t rank)
{
    return PIOL_block_decompose(range_size, num_ranks, rank);
}

/// @brief An overload for a common decomposition case. Perform a decomposition
///        of traces so that the load is optimally balanced across the
///        processes `piol` is operating on.
///
/// @param[in] piol The piol object
/// @param[in] file A read file object. This is used to find the number of
///            traces.
///
/// @return Return a pair (offset, size).
///         The first element is the offset for the local process,
///         the second is the size for the local process.
///
/// @pre piol != NULL
/// @pre file != NULL
///
Contiguous_decomposition block_decompose(ExSeisPIOL* piol, ReadInterface* file);


/// @brief This struct represents the location and local index of a global index
///        in a decomposed range.
struct Decomposed_index_location {
    /// The rank the global index was decomposed onto.
    size_t rank;

    /// The local index on the rank of the range referenced by the global
    /// index.
    size_t local_index;
};


/// @brief Find the rank and local index for a global index of a block decomposed
///        range.
///
/// @param[in] range_size  The size of the decomposed range.
/// @param[in] num_ranks    The number of ranks the range was decomposed over.
/// @param[in] global_index The requested index in the range.
///
/// @returns A Decomposed_index_location containing the `rank` the global_index
///          was decomposed to, and the `local_index` representing the distance
///          between the local range offset and the global index.
///
/// @pre num_ranks > 0
/// @pre global_index < range_size
///
/// @post return.rank < num_ranks
/// @post return.local_index <= global_index
///
Decomposed_index_location block_decomposed_location(
  size_t range_size, size_t num_ranks, size_t global_index);

}  // namespace PIOL

#endif  // __cplusplus

#endif  // EXSEISDAT_PIOL_DECOMPOSE_H
