////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_DISTRIBUTED_VECTOR_HH
#define EXSEISDAT_PIOL_DISTRIBUTED_VECTOR_HH

#include "ExSeisDat/PIOL/decompose.h"
#include "ExSeisDat/PIOL/mpi_utils.hh"
#include "ExSeisDat/PIOL/typedefs.h"

#include <tuple>

namespace PIOL {

/// @brief  A structure for MPI-free accessing using global arrays.
///
/// @tparam T Template parameter pack
///
template<class... T>
class Distributed_vector {
    /// The number of elements per tuple.
    const size_t TupleSz = sizeof(std::tuple<T...>);

    /// The piol object.
    ExSeisPIOL* piol;

    /// The number of elements in the global array.
    size_t sz;

    /// The rank of the local process.
    size_t rank;

    /// The number of ranks.
    size_t numRank;

    /// The local offset.
    size_t offset;

    /// The size of each block per process.
    std::vector<size_t> szall;

    /// The underlying local storage.
    std::vector<std::tuple<T...>> vec;

    /// The memory window.
    MPI_Win win = MPI_WIN_NULL;

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
    /// @param[in] piol_ The PIOL object.
    /// @param[in] sz_ The number of elements in the global array.
    ///
    Distributed_vector(ExSeisPIOL* piol_, const size_t sz_) :
        piol(piol_),
        sz(sz_)
    {
        rank     = piol->comm->getRank();
        numRank  = piol->comm->getNumRank();

        auto dec = decompose(sz, numRank, rank);
        offset   = dec.offset;
        szall    = piol->comm->gather(dec.size);

        vec.resize(dec.size);

        if (numRank > 1) {
            MPI_Win_create(
              vec.data(), vec.size(), TupleSz, MPI_INFO_NULL, MPI_COMM_WORLD,
              &win);
        }
    }

    /// @brief Destruct the global array, free the associated window.
    ///
    /// This is a non-collective operation.
    ///
    ~Distributed_vector(void)
    {
        if (numRank > 1) {
            MPI_Win_free(&win);
        }
    }


    /*! Set the global ith element with the given tuple.
     *  @param[in] i The index into the global array.
     *  @param[in] val The value to be set.
     */
    void set(size_t i, std::tuple<T...> val)
    {
        if (i > sz) return;

        size_t lrank   = 0;
        size_t lOffset = 0;
        for (lrank = 0; i >= lOffset + szall[lrank] && lrank < numRank;
             lrank++) {
            lOffset += szall[lrank];
        }

        if (lrank == rank) {
            vec[i - lOffset] = val;
        }
        else {
            MPI_Win_lock(MPI_LOCK_EXCLUSIVE, lrank, MPI_MODE_NOCHECK, win);
            MPI_Put(
              &val, TupleSz, MPI_utils::MPIType<char>(), lrank, i - lOffset,
              TupleSz, MPI_utils::MPIType<char>(), win);
            MPI_Win_unlock(lrank, win);
        }
    }

    /*! Get the global ith element.
     *  @param[in] i The index into the global array.
     *  @return Return the value of the requested tuple.
     */
    std::tuple<T...> get(size_t i) const
    {
        if (i > sz) {
            std::tuple<T...> empty;
            return empty;
        }
        size_t lrank   = 0;
        size_t lOffset = 0;
        for (lrank = 0; i >= lOffset + szall[lrank] && lrank < numRank;
             lrank++) {
            lOffset += szall[lrank];
        }

        std::tuple<T...> val;
        if (lrank == rank) {
            return vec[i - lOffset];
        }
        else {
            MPI_Win_lock(MPI_LOCK_SHARED, lrank, MPI_MODE_NOCHECK, win);
            MPI_Get(
              &val, TupleSz, MPI_utils::MPIType<char>(), lrank, i - lOffset,
              TupleSz, MPI_utils::MPIType<char>(), win);
            MPI_Win_unlock(lrank, win);
            return val;
        }
    }

    /*! @copydoc get
     */
    std::tuple<T...> operator[](size_t i) const { return get(i); }

    /*! Get the number of elements in the global array.
     *  @return Return the number of elements in the global array.
     */
    size_t size(void) const { return sz; }
};

}  // namespace PIOL

#endif  // EXSEISDAT_PIOL_DISTRIBUTED_VECTOR_HH
