////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/decompose.h"


extern "C" {

PIOL_Contiguous_decomposition PIOL_block_decompose(
  size_t range_size, size_t num_ranks, size_t rank)
{
    assert(num_ranks > 0);

    const size_t chunk_size = range_size / num_ranks;
    const size_t remainder  = range_size % num_ranks;

    // We want to put roughly `chunk_size` elements on every process, and we
    // want to distribute the `remainder` across the processes.
    //
    // We'll distribute the remainder across the lowest ranks.
    // For `rank` < `remainder`, we want to add 1 extra, along with the
    // `chunk_size` to this process. This will put `chunk_size` elements on some
    // processes and `chunk_size+1` on others, covering and fairly distributing
    // the range.
    //
    // In code, the global offset, `global_offset` for rank `rank` will be:
    //     if(rank < remainder) {
    //         // Each rank until this one contains `chunk_size + 1` entries.
    //         global_offset = (chunk_size+1)*rank;
    //     }
    //     else {
    //         // The first `remainder` ranks contain `chunk_size+1` entries,
    //         // and all subsequent ones contain `chunk_size` entries.
    //         global_offset = (chunk_size+1) * remainder
    //                         + chunk_size * (rank - remainder);
    //     }
    // which can be rewritten and expanded
    //     if(rank < remainder)  global_offset = chunk_size * rank + rank;
    //     if(rank >= remainder) global_offset = chunk_size * remainder
    //                                           + remainder
    //                                           + chunk_size * rank
    //                                           - chunk_size * remainder;
    // and simplified
    //     if(rank < remainder)  global_offset = chunk_size * rank + rank;
    //     if(rank >= remainder) global_offset = remainder
    //                                           + chunk_size * rank
    // Given that
    //     rank < remainder  => min(rank, remainder) == rank;
    // and
    //     rank >= remainder => min(rank, remainder) == remainder;
    // we can replace `rank` and `remainder` in the conditionals with the
    // expression `min(rank, remainder)`
    //     if(rank < remainder)  global_offset = chunk_size * rank
    //                                           + min(rank, remainder);
    //     if(rank >= remainder) global_offset = chunk_size * rank
    //                                           + min(rank, remainder);
    //
    // Since both cases are identical, and the conditional cover the whole range
    // of conditions, we can drop the conditionals altogether leaving
    //     global_offset = chunk_size * rank + min(rank, remainder);
    //

    const size_t global_offset = chunk_size * rank + std::min(rank, remainder);

    // The local size is `chunk_size + 1` for the first `remainder` ranks,
    // i.e. if `rank` < `remainder`. Otherwise it is `chunk_size`.
    // The `(rank < remainder)` statement evaluates to `1` when true and `0`
    // when false.
    const size_t local_size = chunk_size + (rank < remainder);

    return {global_offset, local_size};
}

}  // extern "C"


namespace PIOL {

Contiguous_decomposition block_decompose(ExSeisPIOL* piol, ReadInterface* file)
{
    return block_decompose(
      file->readNt(), piol->comm->getNumRank(), piol->comm->getRank());
}

Decomposed_index_location block_decomposed_location(
  size_t range_size, size_t num_ranks, size_t global_index)
{
    assert(global_index < range_size);
    assert(num_ranks > 0);

    // We use the algorithm from `decompose` to find the offset for each rank
    const size_t chunk_size = range_size / num_ranks;
    const size_t remainder  = range_size % num_ranks;

    if (global_index < (chunk_size + 1) * remainder) {
        // If it's on the first `remainder` processes, we can easily
        // calculate the offset, since every process has `chunk_size+1` items.
        const size_t rank        = global_index / (chunk_size + 1);
        const size_t local_index = global_index - (chunk_size + 1) * rank;

        return {rank, local_index};
    }
    else {
        // On the rest of the processes, they have `chunk_size` items.
        // We remove the offset for the processes with `chunk_size+1` items
        // so we can deal directly with `chunk_size`.
        const size_t offset_global_index =
          global_index - (chunk_size + 1) * remainder;

        // Find the rank from the offset index
        const size_t offset_rank = offset_global_index / chunk_size;

        // And find the actual rank by adding the previously dropped ranks.
        const size_t rank = offset_rank + remainder;

        const size_t local_index = global_index - chunk_size * rank - remainder;

        return {rank, local_index};
    }
}

}  // namespace PIOL
