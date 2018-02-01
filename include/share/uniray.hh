////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date May 2017
/// @brief
/// @details
////////////////////////////////////////////////////////////////////////////////
#ifndef PIOLSHAREUNIRAY_INCLUDE_GUARD
#define PIOLSHAREUNIRAY_INCLUDE_GUARD

#include "global.hh"
#include "share/decomp.hh"
#include "share/mpi.hh"

#include <tuple>

namespace PIOL {

/*! A structure for MPI-free accessing using global arrays.
 *  @tparam T Template parameter pack
 */
template<class... T>
class Uniray {
    const size_t TupleSz =
      sizeof(std::tuple<T...>);  //!< The number of elements per tuple.
    ExSeisPIOL* piol;            //!< The piol object.
    size_t sz;                  //!< The number of elements in the global array.
    size_t offset;              //!< The local offset.
    std::vector<size_t> szall;  //!< The size of each block per process.
    std::vector<std::tuple<T...>> vec;  //!< The underlying local storage.

    MPI_Win win;     //!< The memory window.
    size_t rank;     //!< The rank of the local process.
    size_t numRank;  //!< The number of ranks.
  public:
    /*! Construct the global array. This operation is collective across all processes.
     * @param[in] piol_ The PIOL object.
     * @param[in] sz_ The number of elements in the global array.
     */
    Uniray(ExSeisPIOL* piol_, const size_t sz_) : piol(piol_), sz(sz_)
    {
        rank     = piol->comm->getRank();
        numRank  = piol->comm->getNumRank();
        auto dec = decompose(sz, numRank, rank);
        offset   = dec.first;
        szall    = piol->comm->gather(dec.second);
        vec.resize(dec.second);

        if (numRank > 1)
            MPI_Win_create(
              vec.data(), vec.size(), TupleSz, MPI_INFO_NULL, MPI_COMM_WORLD,
              &win);
    }

    /*! Destruct the global array, free the associated window.
     */
    ~Uniray(void)
    {
        if (numRank > 1) MPI_Win_free(&win);
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
        for (lrank = 0; i >= lOffset + szall[lrank] && lrank < numRank; lrank++)
            lOffset += szall[lrank];

        if (lrank == rank)
            vec[i - lOffset] = val;
        else {
            MPI_Win_lock(MPI_LOCK_EXCLUSIVE, lrank, MPI_MODE_NOCHECK, win);
            MPI_Put(
              &val, TupleSz, MPIType<char>(), lrank, i - lOffset, TupleSz,
              MPIType<char>(), win);
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
        for (lrank = 0; i >= lOffset + szall[lrank] && lrank < numRank; lrank++)
            lOffset += szall[lrank];

        std::tuple<T...> val;
        if (lrank == rank)
            return vec[i - lOffset];
        else {
            MPI_Win_lock(MPI_LOCK_SHARED, lrank, MPI_MODE_NOCHECK, win);
            MPI_Get(
              &val, TupleSz, MPIType<char>(), lrank, i - lOffset, TupleSz,
              MPIType<char>(), win);
            MPI_Win_unlock(lrank, win);
            return val;
        }
    }

    /*! Get the global ith element through an overload of the [] operator.
     *  @param[in] i The index into the global array.
     *  @return Return the value of the requested tuple.
     */
    std::tuple<T...> operator[](size_t i) const { return get(i); }

    /*! Get the number of elements in the global array.
     *  @return Return the number of elements in the global array.
     */
    size_t size(void) const { return sz; }
};

}  // namespace PIOL

#endif
