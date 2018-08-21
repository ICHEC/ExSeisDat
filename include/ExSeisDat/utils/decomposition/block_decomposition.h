////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c decompose functions decompose a range over a number of
///        processes.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_DECOMPOSITION_BLOCK_DECOMPOSITION_H
#define EXSEISDAT_UTILS_DECOMPOSITION_BLOCK_DECOMPOSITION_H

#include "ExSeisDat/utils/c_api_utils.h"
#include <stddef.h>

///
/// @namespace exseis::utils::decomposition
///
/// @brief Functions for splitting ranges across processes.
///

#ifdef __cplusplus
namespace exseis {
namespace utils {
inline namespace decomposition {
#endif  // __cplusplus


/// @name C API
///
/// Interfaces visible to a C caller.
///
/// @{

/// @brief The \c Contiguous_decomposition class represents a section of a range
///        on the given rank which has been distributed over a number of ranks.
EXSEISDAT_CXX_ONLY(extern "C")
struct exseis_Contiguous_decomposition {
    /// The offset into the global range for the given rank.
    size_t global_offset;

    /// The local size of the section of the range on the local process
    size_t local_size;
};


/// C API for \ref block_decomposition()
/// @copydoc exseis::utils::decomposition::block_decomposition
EXSEISDAT_CXX_ONLY(extern "C")
struct exseis_Contiguous_decomposition exseis_block_decomposition(
  size_t range_size, size_t num_ranks, size_t rank);

/// @} C API


#ifdef __cplusplus
/// @copydoc exseis_Contiguous_decomposition
struct Contiguous_decomposition : public exseis_Contiguous_decomposition {

    /// @brief Value constructor for Contiguous_decomposition.
    ///
    /// @param[in] global_offset Value for the global_offset member.
    /// @param[in] local_size    Value for the local_size member.
    ///
    Contiguous_decomposition(size_t global_offset, size_t local_size) :
        exseis_Contiguous_decomposition{global_offset, local_size}
    {
    }
};
#endif  // __cplusplus


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
#ifdef __cplusplus
Contiguous_decomposition block_decomposition(
  size_t range_size, size_t num_ranks, size_t rank);
#endif  // __cplusplus


#ifdef __cplusplus
/// @brief This struct represents the location and local index of a global index
///        in a decomposed range.
struct Decomposition_index_location {
    /// The rank the global index was decomposed onto.
    size_t rank;

    /// The local index on the rank of the range referenced by the global
    /// index.
    size_t local_index;
};


/// @brief Find the rank and local index for a global index of a block
///        decomposed range.
///
/// @param[in] range_size   The size of the decomposed range.
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
Decomposition_index_location block_decomposition_location(
  size_t range_size, size_t num_ranks, size_t global_index);

#endif  // __cplusplus


#ifdef __cplusplus
}  // inline namespace decomposition
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_DECOMPOSITION_BLOCK_DECOMPOSITION_H
