/// @file
/// @brief The Distributed_vector_mpi class, an MPI implementation of
///        Distributed_vector.

#ifndef EXSEISDAT_UTILS_DISTRIBUTED_VECTOR_DISTRIBUTED_VECTOR_MPI_HH
#define EXSEISDAT_UTILS_DISTRIBUTED_VECTOR_DISTRIBUTED_VECTOR_MPI_HH

#include "exseisdat/utils/decomposition/block_decomposition.hh"
#include "exseisdat/utils/distributed_vector/Distributed_vector.hh"
#include "exseisdat/utils/mpi_utils/MPI_max_array_length.hh"

#include <mpi.h>

#include <algorithm>
#include <vector>

namespace exseis {
namespace utils {
inline namespace distributed_vector {

/// @brief An MPI implementation of Distributed_vector
template<typename ValueType>
class Distributed_vector_mpi : public Distributed_vector<ValueType> {
  public:
    /// The type to hold
    using value_type = ValueType;


    // Ensure ValueType is trivially copyable, because we'll be passing it
    // around bitwise over MPI.
    static_assert(
        std::is_trivially_copyable<value_type>::value,
        "Template parameter, ValueType, is not trivially copyable!");

  protected:
    /// The internal implementation of Distributed_vector_mpi.
    class Model final : public Distributed_vector<value_type>::Concept {
      public:
        /// The MPI window handle.
        MPI_Win win = MPI_WIN_NULL;

        /// The communicator the window is defined over.
        MPI_Comm comm = MPI_COMM_NULL;

        /// The local data, available for remote access.
        std::vector<value_type> data;

        /// The global size of the array.
        size_t global_size = 0;

        /// @copydoc Distributed_vector_mpi::Distributed_vector_mpi
        Model(MPI_Aint global_size, MPI_Comm comm);

        /// @name @special_member_functions
        /// @{

        /// Destructor
        ~Model() override;

        /// @copy_constructor{delete}
        Model(const Model&) = delete;

        /// @copy_assignment{delete}
        Model& operator=(const Model&) = delete;

        /// @move_constructor{delete}
        Model(Model&&) = delete;

        /// @move_assignment{default semantics}
        /// @param[in] other The instance to move from.
        Model& operator=(Model&& other);

        /// @}

        value_type get(size_t i) const override;
        void set(size_t i, const value_type& val) override;
        void get_n(
            size_t offset, size_t count, value_type* buffer) const override;
        void set_n(
            size_t offset, size_t count, const value_type* buffer) override;
        size_t size() const override;
        void resize(size_t new_size) override;
        void sync() override;
    };

  public:
    /// @brief Construct the global array.
    ///
    /// This operation is collective across all processes.
    ///
    /// The global vector size must be set here, and will remain constant
    /// for the lifetime of the array.
    /// This is primarily because a pointer to the vector data is used in
    /// an MPI window, and resizing that array can invalidate the pointer.
    ///
    /// @param[in] global_size The number of elements in the global array.
    /// @param[in] comm        The communicator to distribute the array over.
    ///
    Distributed_vector_mpi(size_t global_size, MPI_Comm comm);

    /// Import copy/move constructors
    using Distributed_vector<ValueType>::Distributed_vector;
};


template<typename ValueType>
Distributed_vector_mpi<ValueType>::Distributed_vector_mpi(
    size_t global_size, MPI_Comm comm) :
    Distributed_vector<ValueType>(std::make_unique<Model>(global_size, comm))
{
}


template<typename ValueType>
Distributed_vector_mpi<ValueType>::Model::Model(
    MPI_Aint global_size, MPI_Comm comm) :
    comm(comm), global_size(static_cast<size_t>(global_size))
{
    assert(
        global_size >= 0
        && "Distributed_vector_mpi::Model::Model global_size_ should be >= 0.");

    int rank      = 0;
    int num_ranks = 0;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &num_ranks);

    // Check conversions ok
    assert(rank >= 0);
    assert(num_ranks >= 0);

    const auto decomposition = block_decomposition(
        global_size, static_cast<size_t>(num_ranks), static_cast<size_t>(rank));

    // Check conversions ok
    static_assert(
        std::is_unsigned<decltype(decomposition.local_size)>::value,
        "Expected an unsigned integer for decomposition.local_size.");
    assert(decomposition.local_size <= std::numeric_limits<MPI_Aint>::max());

    const auto local_size = static_cast<MPI_Aint>(decomposition.local_size);

    data.resize(local_size);

    MPI_Win_create(
        data.data(), data.size() * sizeof(value_type), sizeof(value_type),
        MPI_INFO_NULL, comm, &win);
}

template<typename ValueType>
Distributed_vector_mpi<ValueType>::Model::~Model()
{
    if (win != MPI_WIN_NULL) {
        MPI_Win_free(&win);
    }
}

template<typename ValueType>
typename Distributed_vector_mpi<ValueType>::Model&
Distributed_vector_mpi<ValueType>::Model::operator=(Model&& other)
{
    if (this != &other) {
        if (win != MPI_WIN_NULL) {
            MPI_Win_free(&win);
        }

        win         = std::move(other.win);
        comm        = std::move(other.comm);
        data        = std::move(other.data);
        global_size = std::move(other.global_size);

        other.win = MPI_WIN_NULL;
    }
    return *this;
}


template<typename ValueType>
ValueType Distributed_vector_mpi<ValueType>::Model::get(size_t i) const
{
    if (i > global_size) {
        return ValueType();
    }

    int rank;
    int num_ranks;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &num_ranks);

    // Check conversions ok
    assert(rank >= 0);
    assert(num_ranks >= 0);

    // Get the rank and local index of the global index `i`.
    auto location = block_decomposition_location(
        global_size, static_cast<size_t>(num_ranks), i);

    // Check conversions to int are in range.
    assert(location.rank <= std::numeric_limits<int>::max());
    assert(location.local_index <= std::numeric_limits<MPI_Aint>::max());

    ValueType val;

    MPI_Win_lock(MPI_LOCK_SHARED, static_cast<int>(location.rank), 0, win);

    const size_t t_size = sizeof(ValueType);
    MPI_Get(
        &val, t_size, MPI_BYTE, static_cast<int>(location.rank),
        static_cast<MPI_Aint>(location.local_index), t_size, MPI_BYTE, win);

    MPI_Win_unlock(static_cast<int>(location.rank), win);

    return val;
}


template<typename ValueType>
void Distributed_vector_mpi<ValueType>::Model::set(
    size_t i, const ValueType& val)
{
    if (i > global_size) {
        return;
    }

    int rank;
    int num_ranks;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &num_ranks);

    // Check conversions ok
    assert(rank >= 0);
    assert(num_ranks >= 0);

    // Get the rank and local index of the global index `i`.
    const auto location = block_decomposition_location(
        global_size, static_cast<size_t>(num_ranks), i);

    // Set the value locally or remotely.
    assert(rank >= 0);
    // Check conversions ok
    assert(location.rank <= std::numeric_limits<int>::max());
    assert(location.local_index <= std::numeric_limits<MPI_Aint>::max());

    // Lock the window, set the remote data, and unlock.
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, static_cast<int>(location.rank), 0, win);

    const size_t t_size = sizeof(ValueType);
    MPI_Put(
        &val, t_size, MPI_BYTE, static_cast<int>(location.rank),
        static_cast<MPI_Aint>(location.local_index), t_size, MPI_BYTE, win);

    MPI_Win_unlock(static_cast<int>(location.rank), win);
}


template<typename ValueType>
void Distributed_vector_mpi<ValueType>::Model::get_n(
    size_t offset, size_t count, ValueType* buffer) const
{
    if (count == 0) return;

    assert(offset + count <= global_size);

    int rank;
    int num_ranks;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &num_ranks);

    // Check conversions ok
    assert(rank >= 0);
    assert(num_ranks >= 0);

    // Loop over the input range on a per-rank basis
    while (count != 0) {
        // Get the rank and local index of the global index `offset`.
        auto location = block_decomposition_location(
            global_size, static_cast<size_t>(num_ranks), offset);


        // Check conversions to int are in range.
        assert(location.rank <= std::numeric_limits<int>::max());
        assert(location.local_index <= std::numeric_limits<MPI_Aint>::max());

        auto rank_chunk = block_decomposition(
            global_size, static_cast<size_t>(num_ranks), location.rank);

        size_t rank_read_size =
            std::min(count, rank_chunk.local_size - location.local_index);

        MPI_Win_lock(MPI_LOCK_SHARED, static_cast<int>(location.rank), 0, win);

        const size_t data_size           = sizeof(ValueType) * rank_read_size;
        const size_t data_chunk_size_max = utils::mpi_max_array_length<char>();
        for (size_t data_chunk_start = 0; data_chunk_start < data_size;
             data_chunk_start += data_chunk_size_max) {
            const size_t data_chunk_end = std::min<size_t>(
                data_chunk_start + data_chunk_size_max, data_size);
            const size_t data_chunk_size = data_chunk_end - data_chunk_start;
            MPI_Get(
                buffer + data_chunk_start, data_chunk_size, MPI_BYTE,
                static_cast<int>(location.rank),
                static_cast<MPI_Aint>(location.local_index + data_chunk_start),
                data_chunk_size, MPI_BYTE, win);
        }

        MPI_Win_unlock(static_cast<int>(location.rank), win);


        // update the offset, count, and buffer pointer for the next read
        offset += rank_read_size;
        count -= rank_read_size;
        buffer += rank_read_size;
    }
}


template<typename ValueType>
void Distributed_vector_mpi<ValueType>::Model::set_n(
    size_t offset, size_t count, const ValueType* buffer)
{
    if (count == 0) return;

    assert(offset + count <= global_size);

    int rank;
    int num_ranks;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &num_ranks);

    // Check conversions ok
    assert(rank >= 0);
    assert(num_ranks >= 0);

    // Loop over the input range on a per-rank basis
    while (count != 0) {
        // Get the rank and local index of the global index `offset`.
        auto location = block_decomposition_location(
            global_size, static_cast<size_t>(num_ranks), offset);


        // Check conversions to int are in range.
        assert(location.rank <= std::numeric_limits<int>::max());
        assert(location.local_index <= std::numeric_limits<MPI_Aint>::max());

        auto rank_chunk = block_decomposition(
            global_size, static_cast<size_t>(num_ranks), location.rank);

        size_t rank_read_size =
            std::min(count, rank_chunk.local_size - location.local_index);

        MPI_Win_lock(MPI_LOCK_SHARED, static_cast<int>(location.rank), 0, win);

        const size_t data_size           = sizeof(ValueType) * rank_read_size;
        const size_t data_chunk_size_max = utils::mpi_max_array_length<char>();
        for (size_t data_chunk_start = 0; data_chunk_start < data_size;
             data_chunk_start += data_chunk_size_max) {
            const size_t data_chunk_end = std::min<size_t>(
                data_chunk_start + data_chunk_size_max, data_size);
            const size_t data_chunk_size = data_chunk_end - data_chunk_start;
            MPI_Put(
                buffer + data_chunk_start, data_chunk_size, MPI_BYTE,
                static_cast<int>(location.rank),
                static_cast<MPI_Aint>(location.local_index + data_chunk_start),
                data_chunk_size, MPI_BYTE, win);
        }

        MPI_Win_unlock(static_cast<int>(location.rank), win);


        // update the offset, count, and buffer pointer for the next read
        offset += rank_read_size;
        count -= rank_read_size;
        buffer += rank_read_size;
    }
}


template<typename ValueType>
size_t Distributed_vector_mpi<ValueType>::Model::size() const
{
    return global_size;
}

template<typename ValueType>
void Distributed_vector_mpi<ValueType>::Model::resize(size_t new_size)
{
    Model tmp(new_size, comm);

    int rank      = -1;
    int num_ranks = -1;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &num_ranks);

    // Check conversions ok
    assert(rank >= 0);
    assert(num_ranks >= 0);

    // Get start and end of local data
    const auto decomposition = block_decomposition(
        global_size, static_cast<size_t>(num_ranks), static_cast<size_t>(rank));

    size_t old_start = decomposition.global_offset;
    size_t old_size  = decomposition.local_size;
    size_t old_end   = old_start + old_size;

    // Get start and end, clamped to the new size. This is needed if
    // the new size is smaller than the old size.
    size_t new_start = std::min(old_start, new_size);
    size_t new_end   = std::min(old_end, new_size);
    for (size_t i = new_start; i < new_end; i++) {
        tmp.set(i, get(i));
    }

    (*this) = std::move(tmp);
}

template<typename ValueType>
void Distributed_vector_mpi<ValueType>::Model::sync()
{
    MPI_Barrier(comm);
}

}  // namespace distributed_vector
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_DISTRIBUTED_VECTOR_DISTRIBUTED_VECTOR_MPI_HH
