/// @file
/// @brief The MPI_Distributed_vector class, an MPI implementation of
///        Distributed_vector.

#ifndef EXSEISDAT_UTILS_MPI_MPI_DISTRIBUTED_VECTOR_HH
#define EXSEISDAT_UTILS_MPI_MPI_DISTRIBUTED_VECTOR_HH

#include "exseisdat/utils/Distributed_vector.hh"
#include "exseisdat/utils/decomposition/block_decomposition.hh"

#include <mpi.h>

namespace exseis {
namespace utils {

/// @brief An MPI implementation of Distributed_vector
template<typename T>
class MPI_Distributed_vector : public Distributed_vector<T> {

    // Ensure T is trivially copyable, because we'll be passing it around
    // bitwise over MPI.
    static_assert(
        std::is_trivially_copyable<T>::value == true,
        "Template parameter, T, is not trivially copyable!");

  protected:
    /// The internal implementation of MPI_Distributed_vector.
    class Model final : public Distributed_vector<T>::Concept {
      public:
        /// The MPI window handle.
        MPI_Win win = MPI_WIN_NULL;

        /// The communicator the window is defined over.
        MPI_Comm comm = MPI_COMM_NULL;

        /// The local data, available for remote access.
        T* data = nullptr;

        /// The global size of the array.
        size_t global_size = 0;

        /// @copydoc MPI_Distributed_vector::MPI_Distributed_vector
        Model(MPI_Aint global_size, MPI_Comm comm);

        ~Model() override;

        void set(size_t i, const T& val) override;
        T get(size_t i) const override;
        size_t size() const override;
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
    MPI_Distributed_vector(size_t global_size, MPI_Comm comm);
};


template<typename T>
MPI_Distributed_vector<T>::MPI_Distributed_vector(
    size_t global_size, MPI_Comm comm) :
    Distributed_vector<T>(std::make_unique<Model>(global_size, comm))
{
}


template<typename T>
MPI_Distributed_vector<T>::Model::Model(MPI_Aint global_size, MPI_Comm comm) :
    comm(comm),
    global_size(static_cast<size_t>(global_size))
{
    assert(
        global_size >= 0
        && "MPI_Distributed_vector::Model::Model global_size_ should be >= 0.");

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

    MPI_Win_allocate(local_size, sizeof(T), MPI_INFO_NULL, comm, &data, &win);
}

template<typename T>
MPI_Distributed_vector<T>::Model::~Model()
{
    MPI_Win_free(&win);
}


template<typename T>
void MPI_Distributed_vector<T>::Model::set(size_t i, const T& val)
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
    if (location.rank == static_cast<size_t>(rank)) {
        data[location.local_index] = val;
    }
    else {
        // Check conversions ok
        assert(location.rank <= std::numeric_limits<int>::max());
        assert(location.local_index <= std::numeric_limits<MPI_Aint>::max());

        // Lock the window, set the remote data, and unlock.
        MPI_Win_lock(
            MPI_LOCK_EXCLUSIVE, static_cast<int>(location.rank),
            MPI_MODE_NOCHECK, win);

        const size_t t_size = sizeof(T);
        MPI_Put(
            &val, t_size, MPI_BYTE, static_cast<int>(location.rank),
            static_cast<MPI_Aint>(location.local_index), t_size, MPI_BYTE, win);

        MPI_Win_unlock(static_cast<int>(location.rank), win);
    }
}

template<typename T>
T MPI_Distributed_vector<T>::Model::get(size_t i) const
{
    if (i > global_size) {
        return T();
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

    // Get the value locally or remotely.
    if (location.rank == static_cast<size_t>(rank)) {
        return data[location.local_index];
    }
    else {
        // Check conversions to int are in range.
        assert(location.rank <= std::numeric_limits<int>::max());
        assert(location.local_index <= std::numeric_limits<MPI_Aint>::max());

        T val;

        MPI_Win_lock(
            MPI_LOCK_SHARED, static_cast<int>(location.rank), MPI_MODE_NOCHECK,
            win);

        const size_t t_size = sizeof(T);
        MPI_Get(
            &val, t_size, MPI_BYTE, static_cast<int>(location.rank),
            static_cast<MPI_Aint>(location.local_index), t_size, MPI_BYTE, win);

        MPI_Win_unlock(static_cast<int>(location.rank), win);

        return val;
    }
}


template<typename T>
size_t MPI_Distributed_vector<T>::Model::size() const
{
    return global_size;
}

}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_MPI_MPI_DISTRIBUTED_VECTOR_HH
