////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief The \c decompose functions decompose a range over a number of
///        processes.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_DECOMPOSITION_BLOCK_DECOMPOSITION_HH
#define EXSEISDAT_UTILS_DECOMPOSITION_BLOCK_DECOMPOSITION_HH

#include <cstddef>

namespace exseis {
namespace utils {
inline namespace decomposition {


///
/// @brief The \c Contiguous_decomposition class represents a section of a range
///        on the given rank which has been distributed over a number of ranks.
///
struct Contiguous_decomposition {
    /// @brief The offset into the global range for the given rank.
    size_t global_offset;

    /// @brief The local size of the section of the range on the local process
    size_t local_size;
};


/// @brief Perform a 1d decomposition of the interval [0,range_size-1] into
///        `num_ranks` pieces so it is optimally spread across each `rank`.
///
/// @param[in] range_size The total size of the 1d domain.
/// @param[in] num_ranks  The number of ranks to perform the decomposition
///                       over.
/// @param[in] rank       The rank of the local process.
///
/// @return The section of the range decomposed onto rank `rank`.
///
/// @pre num_ranks > 0
/// @pre rank < num_ranks
///
Contiguous_decomposition block_decomposition(
    size_t range_size, size_t num_ranks, size_t rank);


/// @brief This struct represents the location and local index of a global index
///        in a decomposed range.
///
struct Decomposition_index_location {
    /// @brief The rank the global index was decomposed onto.
    size_t rank;

    /// @brief The local index on the rank of the range referenced by the global
    ///        index.
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

}  // namespace decomposition
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_DECOMPOSITION_BLOCK_DECOMPOSITION_HH
