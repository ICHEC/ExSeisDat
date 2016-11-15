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

#warning temp
#include <iostream>

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
void Wait(ExSeisPIOL * piol, MPI_Request req1, MPI_Request req2)
{
    MPI_Status stat;
    int err;
    if (piol->comm->getRank() != piol->comm->getNumRank()-1)
    {
        err = MPI_Wait(&req1, &stat);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, &stat, "Sort Rcv error");
    }
    if (piol->comm->getRank())
    {
        err = MPI_Wait(&req2, &stat);
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

    MPI_Request rsnd;
    MPI_Request rrcv;

    if (rank)
    {
        int err = MPI_Irecv(dat.data(), cnt, MPI_CHAR, rank-1, 1, MPI_COMM_WORLD, &rrcv);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Recv error");
    }
    if (rank != piol->comm->getNumRank()-1)
    {
        int err = MPI_Isend(&dat[dat.size()-regionSz], cnt, MPI_CHAR, rank+1, 1, MPI_COMM_WORLD, &rsnd);
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
    MPI_Request rsnd;
    MPI_Request rrcv;

    if (rank)
    {
        int err = MPI_Isend(dat.data(), cnt, MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, &rsnd);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Send error");
    }
    if (rank != piol->comm->getNumRank()-1)
    {
        int err = MPI_Irecv(&dat[dat.size()-regionSz], cnt, MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, &rrcv);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Recv error");
    }
    Wait(piol, rrcv, rsnd);
}

/*! Function to sort a given vector by a nearest neighbour approach.
 *  \tparam T Type of vector
 *  \param[in] piol The PIOL object.
 *  \param[in] nt The total number of objects in all of the vectors across the processes
 *  \param[in,out] dat The vector to sort
 *  \param[in] comp The function to use for less-than comparisons between objects in the
 *                  vector.
 */
template <class T>
void Sort(ExSeisPIOL * piol, size_t nt, std::vector<T> & dat, Compare<T> comp = nullptr)
{
    size_t numRank = piol->comm->getNumRank();
    size_t rank = piol->comm->getRank();
    size_t regionSz = std::min(nt / (numRank * 4U), std::numeric_limits<int>::max() / sizeof(T));
    size_t edge1 = (rank ? regionSz : 0U);
    size_t edge2 = (rank != numRank-1 ? regionSz : 0U);
    std::vector<T> temp1(dat.size() + edge2);
    std::vector<T> temp2(dat.size() + edge2);

    if (comp != nullptr)
        std::sort(dat.begin(), dat.end(), comp);
    else
        std::sort(dat.begin(), dat.end());
    std::copy(dat.begin(), dat.end(), temp1.begin());

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
        for (size_t j = 0; j < dat.size() && !reduced; j++)
            reduced += (temp1[j] != temp2[j]);
        int greduced = 1;

        int err = MPI_Allreduce(&reduced, &greduced, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Allreduce error");

        if (!greduced)
            break;
    }
    for (size_t i = 0; i < dat.size(); i++)
        dat[i] = temp1[i];

}

/*! Function to sort the metadata in a Param struct. The AOSParam structure is initialised
 *  to allow random-access iterator support.
 *  \param[in] piol The PIOL object.
 *  \param[in] nt The total number of objects in all of the vectors across the processes
 *  \param[in] offset The offset for the local process
 *  \param[in,out] prm The parameter structure to sort
 *  \param[in] comp The AOSParam function to use for less-than comparisons between objects in the
 *                  vector.
 *  \return Return the correct order of traces from those which are smallest with respect to the comp function.
 */
std::vector<size_t> Sort(ExSeisPIOL * piol, size_t nt, size_t offset, Param * prm, Compare<AOSParam> comp)
{
    size_t lnt = prm->size();
    auto aos = prm->getAOS();

    Sort(piol, nt, aos, comp);

    std::vector<std::pair<size_t, size_t>> plist(lnt);
    for (size_t i = 0; i < lnt; i++)
    {
        plist[i].first = getPrm<geom_t>(aos[i], Meta::tn);
        //plist[i].first = getPrm<geom_t>(aos[i].j, Meta::tn, aos[i].prm);
        plist[i].second = offset + i;
    }

    Sort(piol, nt, plist);

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
bool lessSrcRcv(const AOSParam & e1, const AOSParam & e2)
{
    size_t j1 = e1.j;
    auto p1 = e1.prm;
    geom_t e1sx = getPrm<geom_t>(j1, Meta::xSrc, p1);

    size_t j2 = e2.j;
    auto p2 = e2.prm;
    geom_t e2sx = getPrm<geom_t>(j2, Meta::xSrc, p2);

    if (e1sx < e2sx)
        return true;
    else if (e1sx == e2sx)
    {
        geom_t e1sy = getPrm<geom_t>(j1, Meta::ySrc, p1);
        geom_t e2sy = getPrm<geom_t>(j2, Meta::ySrc, p2);

        if (e1sy < e2sy)
            return true;
        else if (e1sy == e2sy)
        {
            geom_t e1rx = getPrm<geom_t>(j1, Meta::xRcv, p1);
            geom_t e2rx = getPrm<geom_t>(j2, Meta::xRcv, p2);

            if (e1rx < e2rx)
                return true;
            else if (e1rx == e2rx)
            {
                geom_t e1ry = getPrm<geom_t>(j1, Meta::yRcv, p1);
                geom_t e2ry = getPrm<geom_t>(j2, Meta::yRcv, p2);

                if (e1ry < e2ry)
                    return true;
                else if (e1ry == e2ry)
                    return (getPrm<llint>(j1, Meta::tn, p1) < getPrm<llint>(j2, Meta::tn, p2));
            }
        }
    }
    return false;
}

std::vector<size_t> Sort(ExSeisPIOL * piol, SortType type, size_t nt, size_t offset, Param * prm)
{
    Compare<AOSParam> comp = nullptr;
    switch (type)
    {
        default :
        case SortType::SrcRcv :
        std::cout << "sort src rcv\n";
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
    AOSParam prev;
    for (size_t i = 0; i < dec.second; i++)
    {
        AOSParam ent;
        ent.prm = &prm;
        ent.j = i;

        if (i && !lessSrcRcv(prev, ent))
            return false;
        prev = ent;
    }
    return true;
}
}}

