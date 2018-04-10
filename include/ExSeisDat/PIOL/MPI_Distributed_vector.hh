/// @file
/// @brief The MPI_Distributed_vector class, an MPI implementation of
///        Distributed_vector.

#ifndef EXSEISDAT_PIOL_MPI_DISTRIBUTED_VECTOR_HH
#define EXSEISDAT_PIOL_MPI_DISTRIBUTED_VECTOR_HH

#include "ExSeisDat/PIOL/Distributed_vector.hh"
#include "ExSeisDat/PIOL/decompose.h"

#include <mpi.h>

namespace PIOL {

/// @brief An MPI implementation of Distributed_vector
template<typename T>
class MPI_Distributed_vector : public Distributed_vector<T> {
  protected:
    /// The internal implementation of MPI_Distributed_vector.
    class Model final : public Distributed_vector<T>::Concept {
      public:
        /// The MPI window handle.
        MPI_Win win = MPI_WIN_NULL;

        /// The communicator the window is defined over.
        MPI_Comm comm = MPI_COMM_NULL;

        /// The local data, available for remote access.
        T* data = NULL;

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
    global_size(global_size)
{
    int rank      = 0;
    int num_ranks = 0;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &num_ranks);

    const auto decomposition = decompose_range(global_size, num_ranks, rank);
    const size_t local_size  = decomposition.local_size;

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
    if (i > global_size) return;

    int rank;
    int num_ranks;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &num_ranks);

    // Get the rank and local index of the global index `i`.
    const auto location = decomposed_location(global_size, num_ranks, i);

    // Set the value locally or remotely.
    assert(rank > 0);
    if (location.rank == static_cast<size_t>(rank)) {
        data[location.local_index] = val;
    }
    else {
        // Lock the window, set the remote data, and unlock.
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, location.rank, MPI_MODE_NOCHECK, win);

        const size_t T_size = sizeof(T);
        MPI_Put(
          &val, T_size, MPI_BYTE, location.rank, location.local_index, T_size,
          MPI_BYTE, win);

        MPI_Win_unlock(location.rank, win);
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

    // Get the rank and local index of the global index `i`.
    auto location = decomposed_location(global_size, num_ranks, i);

    // Get the value locally or remotely.
    assert(rank > 0);
    if (location.rank == static_cast<size_t>(rank)) {
        return data[location.local_index];
    }
    else {
        T val;

        MPI_Win_lock(MPI_LOCK_SHARED, location.rank, MPI_MODE_NOCHECK, win);

        const size_t T_size = sizeof(T);
        MPI_Get(
          &val, T_size, MPI_BYTE, location.rank, location.local_index, T_size,
          MPI_BYTE, win);

        MPI_Win_unlock(location.rank, win);

        return val;
    }
}


template<typename T>
size_t MPI_Distributed_vector<T>::Model::size() const
{
    return global_size;
}

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_MPI_DISTRIBUTED_VECTOR_HH
