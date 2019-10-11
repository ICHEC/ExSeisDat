////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief C API for \ref exseisdat/utils/decomposition/block_decomposition.hh
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_DECOMPOSITION_BLOCK_DECOMPOSITION_H
#define EXSEISDAT_UTILS_DECOMPOSITION_BLOCK_DECOMPOSITION_H

#include <stddef.h>


#ifdef __cplusplus
namespace exseis {
namespace utils {
inline namespace decomposition {

extern "C" {
#endif  // __cplusplus


/// @name C API
///
/// @{


///
/// @brief The \c Contiguous_decomposition class represents a section of a range
///        on the given rank which has been distributed over a number of ranks.
///
/// @remark C++ API \ref exseis::utils::decomposition::Contiguous_decomposition
///
///
///
struct exseis_Contiguous_decomposition {
    /// @brief The offset into the global range for the given rank.
    size_t global_offset;

    /// @brief The local size of the section of the range on the local process
    size_t local_size;
};


/// C API for \ref block_decomposition()
/// @copydoc exseis::utils::decomposition::block_decomposition
///
struct exseis_Contiguous_decomposition exseis_block_decomposition(
    size_t range_size, size_t num_ranks, size_t rank);

/// @} C API


#ifdef __cplusplus
}  // extern "C"

}  // namespace decomposition
}  // namespace utils
}  // namespace exseis
#endif  // __cplusplus

#endif  // EXSEISDAT_UTILS_DECOMPOSITION_BLOCK_DECOMPOSITION_H
