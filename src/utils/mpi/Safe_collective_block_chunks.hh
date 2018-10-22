///
/// @file
/// @brief The Safe_collective_block_chunks class is used to split a collective
///        MPI operation over a large array into a number of operations over
///        subsets of the array.
///
#ifndef EXSEISDAT_SRC_UTILS_MPI_SAFE_COLLECTIVE_BLOCK_CHUNKS_HH
#define EXSEISDAT_SRC_UTILS_MPI_SAFE_COLLECTIVE_BLOCK_CHUNKS_HH

#include "exseisdat/utils/mpi/MPI_type.hh"

#include <cassert>
#include <cstddef>

namespace exseis {
namespace utils {
inline namespace mpi {

/// @brief An iterator that represents a square-bracket indexable object.
///
/// This iterator is useful for turning a loop
/// @code
///     for (size_t i = 0; i < objects.size(); i++) {
///         const auto obj = objects[i];
///         // ...
///     }
/// @endcode
/// into
/// @code
///     for (
///         auto it = Indexing_iterator(objects);
///         it != Indexing_iterator(objects, objects.size();
///         it++
///     ) {
///         const auto obj = *it;
///     }
/// @endcode
/// and so, by defining `begin()` and `end()` suitably for the `objects` type:
/// @code
///    for (auto it = begin(objects); it != end(objects); it++) {
///        const auto obj = *it;
///    }
/// @endcode
/// and finally
/// @code
///     for (const auto obj : objects) {
///         // ...
///     }
/// @endcode
///
template<typename T>
class Indexing_iterator {
  public:
    /// The object to index
    const T* value;

    /// The current index of the object
    size_t index = 0;

    /// @brief Construct an Indexing_iterator for an object with the iterator
    ///        starting at a given index.
    ///
    /// @param[in] value The object to index.
    /// @param[in] index The index to start at.
    ///
    Indexing_iterator(const T& value, size_t index = 0) :
        value(std::addressof(value)),
        index(index)
    {
    }


    /// @brief Post-increment the iterator.
    ///
    /// @returns The incremented iterator.
    ///
    Indexing_iterator& operator++()
    {
        index++;
        return *this;
    }


    /// @brief Check inequality between two iterators.
    ///
    /// @param[in] other The rhs value to check inequality with.
    ///
    /// @retval true  if both iterators are at different indices
    /// @retval false otherwise.
    bool operator!=(const Indexing_iterator& other) const
    {
        return index != other.index;
    }


    /// @brief Dereference, i.e., get the object at the given index.
    ///
    /// @returns The value of the object at index i.
    ///
    decltype((*value)[0]) operator*() const { return (*value)[index]; }
};


/// @brief This class provides an interface for splitting up a requested number
///        of blocks into a number of chunks that are safe sizes to use with
///        MPI (due to a bug with the maximum view size that can be set),
///        and a safe number to use with collective calls.
///
/// This class represents a splitting up of the requested number of blocks
/// into a number of safely sized chunks for use with MPI. It also represents
/// the total number of calls needed globally for a collective I/O function.
///
/// It is expected to be used as follows:
/// @code
///     const auto block_chunks = Safe_collective_block_chunks(
///         stride_size, number_of_blocks, max_array_size, communicator)
///
///    for (const auto block_chunk : block_chunks) {
///       const auto offset_into_file   = block_chunk.start * stride_size;
///       const auto offset_into_buffer = block_chunk.start * block_size;
///
///       some_mpi_io_function(
///           block_size, stride_size,
///           block_chunk.size,
///           offset_into_file,
///           &buffer[offset_into_buffer]
///       );
///    }
/// @endcode
/// The trick here is subtle. When one process has a chunk to process, but
/// another has no more data to process, the values of `block_chunk.start` and
/// `block_chunk.size` are both zero on the process with no more data to
/// process.
/// When reading requesting zero bytes or zero blocks from the MPI I/O function,
/// no actual data is read or written to the buffer. However, the process still
/// participates in the collective operation, so MPI doesn't hang.
///
/// The MPI View appears to be limited in the total number of bytes it can
/// consider in the file. This is usually defined by the stride size and the
/// number of blocks being read.
///
/// In some cases where a stride size can't be determined (e.g. reading an
/// irregular list of offsets), a guess must be made.  As a result, while we
/// have a parameter `stride_size` in the constructor, it really represents a
/// "characteristic" distance between the start of blocks.
///
class Safe_collective_block_chunks {

    /// The requested number of blocks to read / write
    size_t m_number_of_blocks;

    /// The maximum safe number of blocks to read / write in a single MPI
    /// call.
    size_t m_blocks_per_call;

    /// The number of calls needed on every process for collective I/O calls.
    size_t m_calls_needed;

  public:
    /// @brief The Constructor
    ///
    /// @param[in] stride_size      The characteristic distance between the
    ///                             start of blocks.
    /// @param[in] number_of_blocks The number of blocks to be read.
    /// @param[in] max_array_size   The maximum array size supported by the MPI
    ///                             implementation.
    /// @param[in] communicator     The mpi communicator the collective
    ///                             operation will be called over.
    ///
    Safe_collective_block_chunks(
      size_t stride_size,
      size_t number_of_blocks,
      size_t max_array_size,
      MPI_Comm communicator)
    {
        m_number_of_blocks = number_of_blocks;

        // Get the maximum number of blocks that `collective_io_function` can be
        // called with at once
        m_blocks_per_call = max_array_size / stride_size;


        // Get the total number of calls needed to `collective_io_function` to
        // satisfy collectiveness. I.e., find out how many times the process
        // with the largest `number_of_blocks` will be calling

        size_t biggest_number_of_blocks = 0;
        MPI_Allreduce(
          &m_number_of_blocks, &biggest_number_of_blocks, 1,
          exseis::utils::mpi_type<size_t>(), MPI_MAX, communicator);

        // helper function for integer division with rounding up
        const auto round_up_divide = [](size_t a, size_t b) {
            assert(b > 0);
            return (a + b - 1) / b;
        };

        // The total number of calls needed to `collective_io_function`
        m_calls_needed =
          round_up_divide(biggest_number_of_blocks, m_blocks_per_call);
    }


    /// @brief A helper class representing a chunk of blocks to be
    ///        read / written.
    ///
    /// This class evaluates to `true` if it contains a chunk of blocks to be
    /// read / written, or `false` if it represents a needed empty call to the
    /// collective MPI function.
    ///
    struct Block_chunk {
        /// The start of the block range
        size_t start = 0;

        /// The size of the block range
        size_t size = 0;
    };


    /// @brief Get the Block_chunk for the `i`th call to the MPI function.
    ///
    /// @param[in] i The Block_chunk to get.
    ///
    /// @returns A Block_chunk representing the range to call the MPI function
    ///          with.
    ///
    Block_chunk operator[](size_t i) const
    {
        assert(i < m_calls_needed);

        // Get starting block index for this call
        const size_t block_start = i * m_blocks_per_call;

        // The block ends at the start of the next block, or at the
        // requested number of blocks. Whichever comes first.
        const size_t block_end =
          std::min((i + 1) * m_blocks_per_call, m_number_of_blocks);

        const size_t block_chunk_size = block_end - block_start;

        if (block_start < m_number_of_blocks) {
            return {block_start, block_chunk_size};
        }
        else {
            return {0, 0};
        }
    }


    /// @brief The number of chunks generated by this class.
    ///
    /// This also represents the expected number of calls needed of a
    /// collective MPI function on every process.
    ///
    /// @return The number of chunks generated by this class.
    ///
    size_t size() const { return m_calls_needed; }


    ///
    /// @name Iterators
    ///

    /// @{

    /// Typedef of an indexing operator to use with the `begin()` and `end()`
    /// functions.
    using Iterator = Indexing_iterator<Safe_collective_block_chunks>;


    /// @brief Get an iterator to the first block chunk.
    ///
    /// @returns An iterator to the first block chunk.
    ///
    Iterator begin() const { return {*this, 0}; }


    /// @brief Get an iterator to one-past-the-end of the block chunks.
    ///
    /// @returns An iterator one-past-the-end of the block chunks.
    ///
    Iterator end() const { return {*this, m_calls_needed}; }

    /// @} Iterators
};

}  // namespace mpi
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_SRC_UTILS_MPI_SAFE_COLLECTIVE_BLOCK_CHUNKS_HH
