/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \date May 2017
 *   \brief
 *   \details
 *//*******************************************************************************************/
#ifndef PIOLSHAREUNIRAY_INCLUDE_GUARD
#define PIOLSHAREUNIRAY_INCLUDE_GUARD
#include <tuple>
#include "global.hh"
#include "share/mpi.hh"
#include "share/decomp.hh"
namespace PIOL {
template <class ... T>
class Uniray
{
    const size_t TupleSz = sizeof(std::tuple<T...>);
    ExSeisPIOL * piol;
    size_t sz;
    size_t offset;
    std::vector<size_t> szall;
    std::vector<std::tuple<T...>> vec;

    MPI_Win win;
    size_t rank, numRank;
    public :
    Uniray(ExSeisPIOL * piol_, size_t sz_) : piol(piol_), sz(sz_)
    {
        rank = piol->comm->getRank();
        numRank = piol->comm->getNumRank();
        auto dec = decompose(sz, numRank, rank);
        offset = dec.first;
        szall = piol->comm->gather(dec.second);
        vec.resize(dec.second);
        MPI_Win_create(vec.data(), vec.size(), TupleSz, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    }

    ~Uniray(void)
    {
        MPI_Win_free(&win);
    }

    void set(size_t i, std::tuple<T...> val)
    {
        if (i > sz)
            return;

        size_t lrank = 0;
        size_t lOffset = 0;
        for (lrank = 0; i >= lOffset + szall[lrank] && lrank < numRank; lrank++)
            lOffset += szall[lrank];

        if (lrank == rank)
            vec[i-lOffset] = val;
        else
        {
            MPI_Win_lock(MPI_LOCK_EXCLUSIVE, lrank, MPI_MODE_NOCHECK, win);
            MPI_Put(&val, TupleSz, MPIType<char>(), lrank, i-lOffset, TupleSz, MPIType<char>(), win);
            MPI_Win_unlock(lrank, win);
        }
    }

    std::tuple<T...> get(size_t i) const
    {
        if (i > sz)
        {
            std::tuple<T...> empty;
            return empty;
        }
        size_t lrank = 0;
        size_t lOffset = 0;
        for (lrank = 0; i >= lOffset + szall[lrank] && lrank < numRank; lrank++)
            lOffset += szall[lrank];

        std::tuple<T...> val;
        if (lrank == rank)
            return vec[i-lOffset];
        else
        {
            MPI_Win_lock(MPI_LOCK_SHARED, lrank, MPI_MODE_NOCHECK, win);
            MPI_Get(&val, TupleSz, MPIType<char>(), lrank, i-lOffset, TupleSz, MPIType<char>(), win);
            MPI_Win_unlock(lrank, win);
            return val;
        }
    }

    std::tuple<T...> operator[](size_t i) const
    {
        return get(i);
    }

    size_t size(void) const
    {
        return sz;
    }
};
}
#endif
