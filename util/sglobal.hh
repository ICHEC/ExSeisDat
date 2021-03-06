#ifndef UTIL_SGLOBALHH_INCLUDE_GUARD
#define UTIL_SGLOBALHH_INCLUDE_GUARD

// POSIX includes
#define _POSIX_C_SOURCE 200809L

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/utils/decomposition/block_decomposition.h"
#include "ExSeisDat/utils/typedefs.h"

#include <stddef.h>

/*! Decomposition which takes account that the data may come in logical blocks
 *  @param[in] sz The size of the problem.
 *  @param[in] bsz The block size
 *  @param[in] numRank The total number of ranks.
 *  @param[in] rank The rank during the function call, generally the rank of the
 *                  calling MPI process.
 *  @return Return an extent, a starting point (generally for a 'for' loop) and
 *          the number of iterations.
 */
exseis::utils::Contiguous_decomposition blockDecomp(
  size_t sz, size_t bsz, size_t numRank, size_t rank, size_t off = 0);

/*! Unequal decomposition, weights towards the lower ranks. Useful for testing
 *  purposes
 *  @param[in] sz The size of the problem.
 *  @param[in] numRank The total number of ranks.
 *  @param[in] rank The rank during the function call, generally the rank of the
 *                  calling MPI process.
 *  @return Return an extent, a starting point (generally for a 'for' loop) and
 *          the number of iterations.
 */
std::vector<size_t> lobdecompose(
  exseis::PIOL::ExSeisPIOL* piol, size_t sz, size_t numRank, size_t rank);

#endif  // UTIL_SGLOBALHH_INCLUDE_GUARD
