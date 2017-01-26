/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date November 2016
 *   \brief The Sort Operation
 *   \details The algorithm used is a nearest neighbour approach where at each iteration
 *   the lowest valued metadata entries are moved to adjacent processes with a lower rank and a
 *   sort is performed. After the sort the highest entries are moved again to the process with a
 *   higher rank. If each process has the same traces it started off with, the sort
 *   is complete.
*//*******************************************************************************************/
#include <algorithm>
#include <iterator>
#include <functional>

#include "global.hh"
#include "ops/sort.hh"
#include "file/file.hh"
#include "share/mpi.hh"

namespace PIOL { namespace File {
/*! Wait on two requests to finish. The largest and smallest rank only wait on one request.
 *  \param[in] piol The PIOL object.
 *  \param[in] req1 The request that all processes except rank of numRank-1 wait on.
 *  \param[in] req2 The request that all processes except rank of 0 wait on..
 */
void Wait(ExSeisPIOL * piol, std::vector<MPI_Request> req1, std::vector<MPI_Request> req2)
{
    MPI_Status stat;
    int err;
    if (piol->comm->getRank() != piol->comm->getNumRank()-1)
        for (size_t i = 0; i < req1.size(); i++)
        {
            err = MPI_Wait(&req1[i], &stat);
            printErr(piol->log.get(), "", Log::Layer::Ops, err, &stat, "Sort Rcv error");
        }
    if (piol->comm->getRank())
        for (size_t i = 0; i < req2.size(); i++)
        {
            err = MPI_Wait(&req2[i], &stat);
            printErr(piol->log.get(), "", Log::Layer::Ops, err, &stat, "Sort Snd error");
        }
}

/*! Send objects from the current processes to the process one rank higher if
 *  such a process exists. Objects are taken from the end of a vector.
 *  Receiving processes put the objects at the start of their vector.
 *  \tparam T Type of vector
 *  \param[in] piol The PIOL object.
 *  \param[in] regionSz The size of data to send/receive.
 *  \param[in,out] dat Vector to be accessed
 */
template <class T>
void sendRight(ExSeisPIOL * piol, size_t regionSz, std::vector<T> & dat)
{
    size_t rank = piol->comm->getRank();
    size_t cnt = regionSz * sizeof(T);
    std::vector<MPI_Request> rsnd(1);
    std::vector<MPI_Request> rrcv(1);

    //TODO: Move to the communication layer?
    if (rank)
    {
        int err = MPI_Irecv(dat.data(), cnt, MPI_CHAR, rank-1, 1, MPI_COMM_WORLD, &rrcv[0]);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Recv error");
    }
    if (rank != piol->comm->getNumRank()-1)
    {
        int err = MPI_Isend(&dat[dat.size()-regionSz], cnt, MPI_CHAR, rank+1, 1, MPI_COMM_WORLD, &rsnd[0]);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Send error");
    }
    Wait(piol, rsnd, rrcv);
}

/*! Send objects from the current processes to the process one rank lower if
 *  such a process exists. Objects are taken from the start of a vector.
 *  Receiving processes put the objects at the end of their vector.
 *  \tparam T Type of vector
 *  \param[in] piol The PIOL object.
 *  \param[in] regionSz The size of data to send/receive.
 *  \param[in,out] dat Vector to be accessed
 */
template <class T>
void sendLeft(ExSeisPIOL * piol, size_t regionSz, std::vector<T> & dat)
{
    size_t rank = piol->comm->getRank();
    size_t cnt = regionSz * sizeof(T);
    std::vector<MPI_Request> rsnd(1);
    std::vector<MPI_Request> rrcv(1);

    if (rank)
    {
        int err = MPI_Isend(dat.data(), cnt, MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, &rsnd[0]);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Send error");
    }
    if (rank != piol->comm->getNumRank()-1)
    {
        int err = MPI_Irecv(&dat[dat.size()-regionSz], cnt, MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, &rrcv[0]);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Recv error");
    }
    Wait(piol, rrcv, rsnd);
}

/*! Send objects from the current processes to the process one rank higher if
 *  such a process exists. Objects are taken from the end of a vector.
 *  Receiving processes put the objects at the start of their vector.
 *  \tparam T Type of vector
 *  \param[in] piol The PIOL object.
 *  \param[in] regionSz The size of data to send/receive.
 *  \param[in,out] dat Vector to be accessed
 */
template <>
void sendRight<Param>(ExSeisPIOL * piol, size_t regionSz, std::vector<Param> & dat)
{
    Param sprm(dat[0].r, regionSz);
    Param rprm(dat[0].r, regionSz);

    size_t rank = piol->comm->getRank();
    std::vector<MPI_Request> rsnd(4);
    std::vector<MPI_Request> rrcv(4);

    Log::Logger * log = piol->log.get();
    auto err = [log] (int errc, std::string msg, size_t i) -> void
        {
            printErr(log, "", Log::Layer::Ops, errc, NULL, msg + std::to_string(i));
        };
    if (rank)
    {
        std::string msg = "Sort right MPI_Irecv error ";
        err(MPI_Irecv(rprm.f.data(), rprm.f.size() * sizeof(geom_t), MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, &rrcv[0]), msg, 1);
        err(MPI_Irecv(rprm.i.data(), rprm.i.size() * sizeof(llint),  MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, &rrcv[1]), msg, 2);
        err(MPI_Irecv(rprm.s.data(), rprm.s.size() * sizeof(short),  MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, &rrcv[2]), msg, 3);
        err(MPI_Irecv(rprm.t.data(), rprm.t.size() * sizeof(size_t), MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, &rrcv[3]), msg, 4);
    }
    if (rank != piol->comm->getNumRank()-1)
    {
        for (size_t i = 0; i < regionSz; i++)
            cpyPrm(0U, &dat[i+dat.size()-regionSz], i, &sprm);

        std::string msg = "Sort right MPI_Isend error ";
        err(MPI_Isend(sprm.f.data(), sprm.f.size() * sizeof(geom_t), MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, &rsnd[0]), msg, 1);
        err(MPI_Isend(sprm.i.data(), sprm.i.size() * sizeof(llint),  MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, &rsnd[1]), msg, 2);
        err(MPI_Isend(sprm.s.data(), sprm.s.size() * sizeof(short),  MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, &rsnd[2]), msg, 3);
        err(MPI_Isend(sprm.t.data(), sprm.t.size() * sizeof(size_t), MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, &rsnd[3]), msg, 4);
    }
    Wait(piol, rsnd, rrcv);

    if (rank)
        for (size_t i = 0; i < regionSz; i++)
            cpyPrm(i, &rprm, 0U, &dat[i]);
}

/*! Send objects from the current processes to the process one rank lower if
 *  such a process exists. Objects are taken from the start of a vector.
 *  Receiving processes put the objects at the end of their vector.
 *  \tparam T Type of vector
 *  \param[in] piol The PIOL object.
 *  \param[in] regionSz The size of data to send/receive.
 *  \param[in,out] dat Vector to be accessed
 */
template <>
void sendLeft<Param>(ExSeisPIOL * piol, size_t regionSz, std::vector<Param> & dat)
{
    Param sprm(dat[0].r, regionSz);
    Param rprm(dat[0].r, regionSz);

    size_t rank = piol->comm->getRank();
    std::vector<MPI_Request> rsnd(4);
    std::vector<MPI_Request> rrcv(4);

    Log::Logger * log = piol->log.get();
    auto err = [log] (int errc, std::string msg, size_t i) -> void
        {
            printErr(log, "", Log::Layer::Ops, errc, NULL, msg + std::to_string(i));
        };

    if (rank)
    {
        for (size_t i = 0; i < regionSz; i++)
            cpyPrm(0U, &dat[i], i, &sprm);

        std::string msg = "Sort left MPI_Irecv error ";
        err(MPI_Isend(sprm.f.data(), sprm.f.size() * sizeof(geom_t), MPI_CHAR, rank-1, 1, MPI_COMM_WORLD, &rsnd[0]), msg, 1);
        err(MPI_Isend(sprm.i.data(), sprm.i.size() * sizeof(llint),  MPI_CHAR, rank-1, 1, MPI_COMM_WORLD, &rsnd[1]), msg, 2);
        err(MPI_Isend(sprm.s.data(), sprm.s.size() * sizeof(short),  MPI_CHAR, rank-1, 1, MPI_COMM_WORLD, &rsnd[2]), msg, 3);
        err(MPI_Isend(sprm.t.data(), sprm.t.size() * sizeof(size_t), MPI_CHAR, rank-1, 1, MPI_COMM_WORLD, &rsnd[3]), msg, 4);
    }
    if (rank != piol->comm->getNumRank()-1)
    {
        std::string msg = "Sort left MPI_Isend error ";
        err(MPI_Irecv(rprm.f.data(), rprm.f.size() * sizeof(geom_t), MPI_CHAR, rank+1, 1, MPI_COMM_WORLD, &rrcv[0]), msg, 1);
        err(MPI_Irecv(rprm.i.data(), rprm.i.size() * sizeof(llint),  MPI_CHAR, rank+1, 1, MPI_COMM_WORLD, &rrcv[1]), msg, 2);
        err(MPI_Irecv(rprm.s.data(), rprm.s.size() * sizeof(short),  MPI_CHAR, rank+1, 1, MPI_COMM_WORLD, &rrcv[2]), msg, 3);
        err(MPI_Irecv(rprm.t.data(), rprm.t.size() * sizeof(size_t), MPI_CHAR, rank+1, 1, MPI_COMM_WORLD, &rrcv[3]), msg, 4);
    }

    Wait(piol, rrcv, rsnd);

    if (rank != piol->comm->getNumRank()-1)
        for (size_t i = 0; i < regionSz; i++)
            cpyPrm(i, &rprm, 0U, &dat[i+dat.size()-regionSz]);
}

/*! Function to sort a given vector by a nearest neighbour approach.
 *  \tparam T Type of vector
 *  \param[in] piol The PIOL object.
 *  \param[in] regionSz The size of the region which will be shared
 *  \param[in] temp1 Temporary vector which is the dat.size()+regionSz.
 *  \param[in,out] dat The vector to sort
 *  \param[in] comp The function to use for less-than comparisons between objects in the
 *                  vector.
 */
template <class T>
void sort(ExSeisPIOL * piol, size_t regionSz, std::vector<T> & temp1, std::vector<T> & dat, Compare<T> comp = nullptr)
{
    size_t lnt = dat.size();
    size_t numRank = piol->comm->getNumRank();
    size_t rank = piol->comm->getRank();
    size_t edge1 = (rank ? regionSz : 0U);
    size_t edge2 = (rank != numRank-1 ? regionSz : 0U);

    if (comp != nullptr)
        std::sort(dat.begin(), dat.begin() + lnt, [comp](auto & a, auto & b) -> bool { return comp(a, b); });
    else
        std::sort(dat.begin(), dat.begin() + lnt);

    std::copy(dat.begin(), dat.end(), temp1.begin());
    std::vector<T> temp2;
    while (numRank > 1) //Infinite loop if there is more than one process, otherwise no loop
    {
        temp2 = temp1;
        sendLeft(piol, regionSz, temp1);

        if (comp != nullptr)
            std::sort(temp1.begin() + edge1, temp1.end(), [comp](auto & a, auto & b) -> bool { return comp(a, b); });
        else
            std::sort(temp1.begin() + edge1, temp1.end()); // default pair sorting is first then second

        sendRight(piol, regionSz, temp1);

        if (comp != nullptr)
            std::sort(temp1.begin(), temp1.end() - edge2, [comp](auto & a, auto & b) -> bool { return comp(a, b); });
        else
            std::sort(temp1.begin(), temp1.end() - edge2);

        int reduced = 0;
        for (size_t j = 0; j < lnt && !reduced; j++)
            reduced += (temp1[j] != temp2[j]);
        int greduced = 1;

        int err = MPI_Allreduce(&reduced, &greduced, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Allreduce error");

        if (!greduced)
            break;
    }
    for (size_t i = 0; i < lnt; i++)
        dat[i] = temp1[i];
}

/*! Function to sort the metadata in a Param struct. The Param vector is used internally
 *  to allow random-access iterator support.
 *  \param[in] piol The PIOL object.
 *  \param[in] offset The offset for the local process
 *  \param[in,out] prm The parameter structure to sort
 *  \param[in] comp The Param function to use for less-than comparisons between objects in the
 *                  vector. It assumes each Param structure has exactly one entry.
 *  \return Return the correct order of traces from those which are smallest with respect to the comp function.
 *          i.e. returns global offset positions of traces in sorted order
 */
std::vector<size_t> sort(ExSeisPIOL * piol, size_t offset, Param * prm, Compare<Param> comp)
{
    size_t lnt = prm->size();
    size_t memSz = (prm->f.size() + prm->i.size() + prm->s.size() + prm->t.size() + sizeof(Param) + sizeof(std::pair<size_t, size_t>)) / prm->size();
    size_t regionSz = std::min(piol->comm->min(lnt) / 4U, getLimSz(memSz));

    size_t edge2 = (piol->comm->getRank() != piol->comm->getNumRank()-1 ? regionSz : 0U);
    std::vector<Param> vprm;

    for (size_t i = 0; i < lnt; i++)
    {
        vprm.emplace_back(prm->r, 1U);
        cpyPrm(i, prm, 0, &vprm.back());
    }

    {
        std::vector<Param> temp1; //The extra vector temp1 is needed to be larger than vprm for passing values to neighbours
        for (size_t i = 0; i < lnt+edge2; i++)
            temp1.emplace_back(prm->r, 1U);

        sort(piol, regionSz, temp1, vprm, comp);
    }

    std::vector<std::pair<size_t, size_t>> plist(lnt);
    for (size_t i = 0; i < lnt; i++)
    {
        plist[i].first = getPrm<llint>(0U, Meta::gtn, &vprm[i]);
        plist[i].second = offset + i;
    }

    {
        std::vector<std::pair<size_t, size_t>> temp1(lnt+edge2);

        //Work-around for bug in the intel compiler (intel/2016-u3) on Fionn with std::pair less-than operator
        #if defined(__INTEL_COMPILER)
        Compare<std::pair<size_t, size_t>> check = [](const std::pair<size_t, size_t> & e1, const std::pair<size_t, size_t> & e2) -> bool
            {
                return (e1.first < e2.first || (e1.first == e2.first && e1.second < e2.second));
            };
        sort(piol, regionSz, temp1, plist, check);
        #else
        sort(piol, regionSz, temp1, plist);
        #endif
    }

    std::vector<size_t> list(lnt);
    for (size_t i = 0; i < lnt; i++)
        list[i] = plist[i].second;
    return list;
}
}}
