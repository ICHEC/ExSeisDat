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
#include "ops/ops.hh"
#include "file/file.hh"
#include "share/smpi.hh"

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
 *  \param[in] nt The total number of objects in all of the vectors across the processes
 *  \param[in,out] dat The vector to sort
 *  \param[in] comp The function to use for less-than comparisons between objects in the
 *                  vector.
 */
template <class T>
void Sort(ExSeisPIOL * piol, size_t regionSz, std::vector<T> & temp1, std::vector<T> & dat, Compare<T> comp = nullptr)
{
    size_t lnt = dat.size();
    size_t numRank = piol->comm->getNumRank();
    size_t rank = piol->comm->getRank();
    size_t edge1 = (rank ? regionSz : 0U);
    size_t edge2 = (rank != numRank-1 ? regionSz : 0U);

    if (comp != nullptr)
        std::sort(dat.begin(), dat.begin() + lnt, comp);
    else
        std::sort(dat.begin(), dat.begin() + lnt);

    std::copy(dat.begin(), dat.end(), temp1.begin());
    std::vector<T> temp2;
    while (numRank > 1) //Infinite loop if there is more than one process, otherwise no loop
    {
        temp2 = temp1;
        sendLeft(piol, regionSz, temp1);

        if (comp != nullptr)
            std::sort(temp1.begin() + edge1, temp1.end(), comp);
        else
            std::sort(temp1.begin() + edge1, temp1.end());

        sendRight(piol, regionSz, temp1);

        if (comp != nullptr)
            std::sort(temp1.begin(), temp1.end() - edge2, comp);
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
 */
std::vector<size_t> Sort(ExSeisPIOL * piol, size_t nt, size_t offset, Param * prm, Compare<Param> comp)
{
    size_t lnt = prm->size();
    size_t memSz = (prm->f.size() + prm->i.size() + prm->s.size() + prm->t.size() + sizeof(Param) + sizeof(std::pair<size_t, size_t>)) / prm->size();
    size_t regionSz = std::min(nt / (piol->comm->getNumRank() * 4U), getLimSz(memSz));
    size_t edge2 = (piol->comm->getRank() != piol->comm->getNumRank()-1 ? regionSz : 0U);
    std::vector<Param> vprm;

    for (size_t i = 0; i < lnt; i++)
    {
        vprm.emplace_back(prm->r, 1U);
        cpyPrm(i, prm, 0, &vprm.back());
    }

    {
        std::vector<Param> temp1;
        for (size_t i = 0; i < lnt+edge2; i++)
            temp1.emplace_back(prm->r, 1U);

        Sort(piol, regionSz, temp1, vprm, comp);
    }

    std::vector<std::pair<size_t, size_t>> plist(lnt);
    for (size_t i = 0; i < lnt; i++)
    {
        plist[i].first = getPrm<llint>(0U, Meta::tn, &vprm[i]);
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
        Sort(piol, regionSz, temp1, plist, check);
        #else
        Sort(piol, regionSz, temp1, plist, check);
        #endif
    }

    std::vector<size_t> list(lnt);
    for (size_t i = 0; i < lnt; i++)
        list[i] = plist[i].second;
    return list;
}

/*! Calculate the square of the hypotenuse
 *  \param[in] sx The source x coordinate
 *  \param[in] sy The source y coordinate
 *  \param[in] rx The receiver x coordinate
 *  \param[in] ry The receiver y coordinate
 *  \return square of the hypotenuse
 */
inline geom_t off(geom_t sx, geom_t sy, geom_t rx, geom_t ry)
{
    return (sx-rx)*(sx-rx) + (sy-ry)*(sy-ry);
}

/*! For sorting by Src X, Src Y, Rcv X, Rcv Y.
 *  \param[in] e1 Structure to access jth parameter of associated Param struct.
 *  \param[in] e2 Structure to access jth parameter of associated Param struct.
 *  \return Return true if e1 is less than e2 in terms of the sort.
 */
bool lessSrcRcv(const Param & e1, const Param & e2)
{
    geom_t e1sx = getPrm<geom_t>(0U, Meta::xSrc, &e1);
    geom_t e2sx = getPrm<geom_t>(0U, Meta::xSrc, &e2);

    if (e1sx < e2sx)
        return true;
    else if (e1sx == e2sx)
    {
        geom_t e1sy = getPrm<geom_t>(0U, Meta::ySrc, &e1);
        geom_t e2sy = getPrm<geom_t>(0U, Meta::ySrc, &e2);

        if (e1sy < e2sy)
            return true;
        else if (e1sy == e2sy)
        {
            geom_t e1rx = getPrm<geom_t>(0U, Meta::xRcv, &e1);
            geom_t e2rx = getPrm<geom_t>(0U, Meta::xRcv, &e2);

            if (e1rx < e2rx)
                return true;
            else if (e1rx == e2rx)
            {
                geom_t e1ry = getPrm<geom_t>(0U, Meta::yRcv, &e1);
                geom_t e2ry = getPrm<geom_t>(0U, Meta::yRcv, &e2);

                if (e1ry < e2ry)
                    return true;
                else if (e1ry == e2ry)
                    return (getPrm<llint>(0U, Meta::tn, &e1) < getPrm<llint>(0U, Meta::tn, &e2));
            }
        }
    }
    return false;
}

std::vector<size_t> Sort(ExSeisPIOL * piol, SortType type, size_t nt, size_t offset, Param * prm)
{
    Compare<Param> comp = nullptr;
    switch (type)
    {
        default :
        case SortType::SrcRcv :
        comp = lessSrcRcv;
        break;
        case SortType::OffsetLine :
#warning To be done during the next visit
        break;
        case SortType::CmpSrc :
#warning To be done during the next visit
        break;
    }
    return Sort(piol, nt, offset, prm, comp);
}

//TODO: Make this work with SortType type;
bool checkOrder(Interface * src, std::pair<size_t , size_t> dec)
{
    Param prm(dec.second);
    src->readParam(dec.first, dec.second, &prm);
    Param prev(prm.r, 1);
    Param next(prm.r, 1);
    for (size_t i = 0; i < dec.second; i++)
    {
        cpyPrm(i, &prm, 0U, &next);

        if (i && !lessSrcRcv(prev, next))
            return false;
        std::swap(prev, next);
    }
    return true;
}
}}

