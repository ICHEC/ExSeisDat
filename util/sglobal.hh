#ifndef EXSEISDAT_UTIL_SGLOBAL_HH
#define EXSEISDAT_UTIL_SGLOBAL_HH

#include "exseisdat/piol/ExSeisPIOL.hh"
#include "exseisdat/utils/decomposition/block_decomposition.hh"
#include "exseisdat/utils/typedefs.hh"

#include <stddef.h>

/*! Decomposition which takes account that the data may come in logical blocks
 *  @param[in] sz The size of the problem.
 *  @param[in] bsz The block size
 *  @param[in] num_rank The total number of ranks.
 *  @param[in] rank The rank during the function call, generally the rank of the
 *                  calling MPI process.
 *  @return Return an extent, a starting point (generally for a 'for' loop) and
 *          the number of iterations.
 */
exseis::utils::Contiguous_decomposition block_decomp(
  size_t sz, size_t bsz, size_t num_rank, size_t rank, size_t off = 0);

/*! Unequal decomposition, weights towards the lower ranks. Useful for testing
 *  purposes
 *  @param[in] sz The size of the problem.
 *  @param[in] num_rank The total number of ranks.
 *  @param[in] rank The rank during the function call, generally the rank of the
 *                  calling MPI process.
 *  @return Return an extent, a starting point (generally for a 'for' loop) and
 *          the number of iterations.
 */
std::vector<size_t> lobdecompose(
  exseis::piol::ExSeisPIOL* piol, size_t sz, size_t num_rank, size_t rank);

#endif  // EXSEISDAT_UTIL_SGLOBAL_HH
