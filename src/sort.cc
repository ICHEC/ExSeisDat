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
#include "global.hh"
#include "ops/sort.hh"
#include "file/dynsegymd.hh"
#include "share/mpi.hh"
#include "share/api.hh"

namespace PIOL { namespace File {

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
 *  \param[in] e1 Structure to access 0th parameter of associated Param struct.
 *  \param[in] e2 Structure to access 0th parameter of associated Param struct.
 *  \return Return true if e1 is less than e2 in terms of the sort.
 */
bool lessSrcRcv(const Param * prm, csize_t i, csize_t j)
{
    auto e1sx = getPrm<geom_t>(i, Meta::xSrc, prm);
    auto e2sx = getPrm<geom_t>(j, Meta::xSrc, prm);

    if (e1sx < e2sx)
        return true;
    else if (e1sx == e2sx)
    {
        auto e1sy = getPrm<geom_t>(i, Meta::ySrc, prm);
        auto e2sy = getPrm<geom_t>(j, Meta::ySrc, prm);

        if (e1sy < e2sy)
            return true;
        else if (e1sy == e2sy)
        {
            auto e1rx = getPrm<geom_t>(i, Meta::xRcv, prm);
            auto e2rx = getPrm<geom_t>(j, Meta::xRcv, prm);

            if (e1rx < e2rx)
                return true;
            else if (e1rx == e2rx)
            {
                auto e1ry = getPrm<geom_t>(i, Meta::yRcv, prm);
                auto e2ry = getPrm<geom_t>(j, Meta::yRcv, prm);

                if (e1ry < e2ry)
                    return true;
                else if (e1ry == e2ry)
                    return (getPrm<llint>(i, Meta::ltn, prm) < getPrm<llint>(j, Meta::ltn, prm));
            }
        }
    }
    return false;
}

/*! For sorting by Src X, Src Y and Offset.
 *  \tparam CalcOff If true, calculate the offset, otherwise read the offset from the header
 *  \param[in] e1 Structure to access 0th parameter of associated Param struct.
 *  \param[in] e2 Structure to access 0th parameter of associated Param struct.
 *  \return Return true if e1 is less than e2 in terms of the sort.
 */
template <bool CalcOff>
bool lessSrcOff(const Param * prm, csize_t i, csize_t j)
{
    auto e1sx = getPrm<geom_t>(i, Meta::xSrc, prm);
    auto e2sx = getPrm<geom_t>(j, Meta::xSrc, prm);

    if (e1sx < e2sx)
        return true;
    else if (e1sx == e2sx)
    {
        auto e1sy = getPrm<geom_t>(i, Meta::ySrc, prm);
        auto e2sy = getPrm<geom_t>(j, Meta::ySrc, prm);

        if (e1sy < e2sy)
            return true;
        else if (e1sy == e2sy)
        {
            auto e1rx = getPrm<geom_t>(i, Meta::xRcv, prm);
            auto e1ry = getPrm<geom_t>(i, Meta::yRcv, prm);
            auto off1 = (CalcOff ? off(e1sx, e1sy, e1rx, e1ry) : getPrm<size_t>(i, Meta::Offset, prm));

            auto e2rx = getPrm<geom_t>(j, Meta::xRcv, prm);
            auto e2ry = getPrm<geom_t>(j, Meta::yRcv, prm);
            auto off2 = (CalcOff ? off(e2sx, e2sy, e2rx, e2ry) : getPrm<size_t>(j, Meta::Offset, prm));

            return (off1 < off2 || (off1 == off2 && getPrm<llint>(i, Meta::ltn, prm) < getPrm<llint>(j, Meta::ltn, prm)));
        }
    }
    return false;
}

/*! For sorting by Rcv X, Rcv Y and Offset.
 *  \tparam CalcOff If true, calculate the offset, otherwise read the offset from the header
 *  \param[in] e1 Structure to access 0th parameter of associated Param struct.
 *  \param[in] e2 Structure to access 0th parameter of associated Param struct.
 *  \return Return true if e1 is less than e2 in terms of the sort.
 */
template <bool CalcOff>
bool lessRcvOff(const Param * prm, csize_t i, csize_t j)
{
    auto e1rx = getPrm<geom_t>(i, Meta::xRcv, prm);
    auto e2rx = getPrm<geom_t>(j, Meta::xRcv, prm);

    if (e1rx < e2rx)
        return true;
    else if (e1rx == e2rx)
    {
        auto e1ry = getPrm<geom_t>(i, Meta::yRcv, prm);
        auto e2ry = getPrm<geom_t>(j, Meta::yRcv, prm);

        if (e1ry < e2ry)
            return true;
        else if (e1ry == e2ry)
        {
            auto e1sx = getPrm<geom_t>(i, Meta::xSrc, prm);
            auto e1sy = getPrm<geom_t>(i, Meta::ySrc, prm);
            auto off1 = (CalcOff ? off(e1sx, e1sy, e1rx, e1ry) : getPrm<size_t>(i, Meta::Offset, prm));

            auto e2sx = getPrm<geom_t>(j, Meta::xSrc, prm);
            auto e2sy = getPrm<geom_t>(j, Meta::ySrc, prm);
            auto off2 = (CalcOff ? off(e2sx, e2sy, e2rx, e2ry) : getPrm<size_t>(j, Meta::Offset, prm));

            return (off1 < off2 || (off1 == off2 && getPrm<llint>(i, Meta::ltn, prm) < getPrm<llint>(j, Meta::ltn, prm)));
        }
    }
    return false;
}

/*! For sorting by Inline, Crossline and Offset.
 *  \tparam CalcOff If true, calculate the offset, otherwise read the offset from the header
 *  \param[in] e1 Structure to access 0th parameter of associated Param struct.
 *  \param[in] e2 Structure to access 0th parameter of associated Param struct.
 *  \return Return true if e1 is less than e2 in terms of the sort.
 */
template <bool CalcOff>
bool lessLineOff(const Param * prm, csize_t i, csize_t j)
{
    auto e1il = getPrm<llint>(i, Meta::il, prm);
    auto e2il = getPrm<llint>(j, Meta::il, prm);

    if (e1il < e2il)
        return true;
    else if (e1il == e2il)
    {
        auto e1xl = getPrm<llint>(i, Meta::xl, prm);
        auto e2xl = getPrm<llint>(j, Meta::xl, prm);
        if (e1xl < e2xl)
            return true;
        else if (e1xl == e2xl)
        {
            auto e1sx = getPrm<geom_t>(i, Meta::xSrc, prm);
            auto e1sy = getPrm<geom_t>(i, Meta::ySrc, prm);
            auto e1rx = getPrm<geom_t>(i, Meta::xRcv, prm);
            auto e1ry = getPrm<geom_t>(i, Meta::yRcv, prm);

            auto e2sx = getPrm<geom_t>(j, Meta::xSrc, prm);
            auto e2sy = getPrm<geom_t>(j, Meta::ySrc, prm);
            auto e2rx = getPrm<geom_t>(j, Meta::xRcv, prm);
            auto e2ry = getPrm<geom_t>(j, Meta::yRcv, prm);

            auto off1 = (CalcOff ? off(e1sx, e1sy, e1rx, e1ry) : getPrm<size_t>(i, Meta::Offset, prm));
            auto off2 = (CalcOff ? off(e2sx, e2sy, e2rx, e2ry) : getPrm<size_t>(j, Meta::Offset, prm));
            return (off1 < off2 || (off1 == off2 && getPrm<llint>(i, Meta::ltn, prm) < getPrm<llint>(j, Meta::ltn, prm)));
        }
    }
    return false;
}

/*! For sorting by Offset, Inline and Crossline.
 *  \tparam CalcOff If true, calculate the offset, otherwise read the offset from the header
 *  \param[in] e1 Structure to access 0th parameter of associated Param struct.
 *  \param[in] e2 Structure to access 0th parameter of associated Param struct.
 *  \return Return true if e1 is less than e2 in terms of the sort.
 */
template <bool CalcOff>
bool lessOffLine(const Param * prm, csize_t i, csize_t j)
{
    auto e1sx = getPrm<geom_t>(i, Meta::xSrc, prm);
    auto e1sy = getPrm<geom_t>(i, Meta::ySrc, prm);
    auto e1rx = getPrm<geom_t>(i, Meta::xRcv, prm);
    auto e1ry = getPrm<geom_t>(i, Meta::yRcv, prm);
    auto off1 = (CalcOff ? off(e1sx, e1sy, e1rx, e1ry) : getPrm<size_t>(i, Meta::Offset, prm));

    auto e2sx = getPrm<geom_t>(j, Meta::xSrc, prm);
    auto e2sy = getPrm<geom_t>(j, Meta::ySrc, prm);
    auto e2rx = getPrm<geom_t>(j, Meta::xRcv, prm);
    auto e2ry = getPrm<geom_t>(j, Meta::yRcv, prm);
    auto off2 = (CalcOff ? off(e2sx, e2sy, e2rx, e2ry) : getPrm<size_t>(j, Meta::Offset, prm));

    if (off1 < off2)
        return true;
    else if (off1 == off2)
    {
        auto e1il = getPrm<llint>(i, Meta::il, prm);
        auto e2il = getPrm<llint>(j, Meta::il, prm);
        if (e1il < e2il)
            return true;
        else if (e1il == e2il)
        {
            auto e1xl = getPrm<llint>(i, Meta::xl, prm);
            auto e2xl = getPrm<llint>(j, Meta::xl, prm);
            return (e1xl < e2xl || (e1xl == e2xl && getPrm<llint>(i, Meta::ltn, prm) < getPrm<llint>(j, Meta::ltn, prm)));
        }
    }
    return false;
}

CompareP getComp(SortType type)
{
    switch (type)
    {
        default :
        case SortType::SrcRcv :
            return lessSrcRcv;
        break;
        case SortType::SrcOff :
            return lessSrcOff<true>;
        break;
        case SortType::SrcROff :
            return lessSrcOff<false>;
        break;
        case SortType::RcvOff :
            return lessRcvOff<true>;
        break;
        case SortType::RcvROff :
            return lessRcvOff<false>;
        break;
        case SortType::LineOff :
            return lessLineOff<true>;
        break;
        case SortType::LineROff :
            return lessLineOff<false>;
        break;
        case SortType::OffLine :
            return lessOffLine<true>;
        break;
        case SortType::ROffLine :
            return lessOffLine<false>;
        break;
    }
}

std::vector<size_t> sort(ExSeisPIOL * piol, SortType type, Param * prm)
{
    return sort(piol, prm, getComp(type));
}

bool checkOrder(ReadInterface * src, std::pair<size_t , size_t> dec, SortType type)
{
    auto comp = getComp(type);
    Param prm(dec.second);
    src->readParam(dec.first, dec.second, &prm);
    for (size_t i = 1; i < dec.second; i++)
        if (!comp(&prm, i-1, i))
            return false;
    return true;
}

/************************** Core Implementation **************************************/

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

/*! Function to sort a given vector by a nearest neighbour approach.
 *  \tparam T Type of vector
 *  \param[in] piol The PIOL object.
 *  \param[in] regionSz The size of the region which will be shared
 *  \param[in, out] temp1 Temporary vector which is the dat.size()+regionSz.
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
    size_t edge1 = (rank ? regionSz : 0LU);
    size_t edge2 = (rank != numRank-1 ? regionSz : 0LU);

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

/*! Parallel sort a list. Local vector is part of the entire list.
 *  \param[in] piol The PIOL object.
 *  \param[in] list The local vector
 *  \return Return a new sorted vector
 */
std::vector<size_t> sort(ExSeisPIOL * piol, std::vector<size_t> list)
{
    csize_t lnt = list.size();
    csize_t regionSz = piol->comm->min(lnt) / 4LU;
    csize_t edge2 = (piol->comm->getRank() != piol->comm->getNumRank()-1 ? regionSz : 0LU);
    csize_t offset = piol->comm->offset(lnt);

    std::vector<std::pair<size_t, size_t>> plist(lnt);
    for (size_t i = 0; i < lnt; i++)
    {
        plist[i].first = list[i];
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

    for (size_t i = 0; i < lnt; i++)
        list[i] = plist[i].second;
    return list;
}

/*! Send objects from the current processes to the process one rank higher if
 *  such a process exists. Objects are taken from the end of a vector.
 *  Receiving processes put the objects at the start of their vector.
 *  \tparam T Type of vector
 *  \param[in] piol The PIOL object.
 *  \param[in] regionSz The size of data to send/receive.
 *  \param[in,out] dat Vector to be accessed
 */
void sendRight(ExSeisPIOL * piol, size_t regionSz, Param * prm)
{
    Param sprm(prm->r, regionSz);
    Param rprm(prm->r, regionSz);

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
            cpyPrm(i+prm->size()-regionSz, prm, i, &sprm);

        std::string msg = "Sort right MPI_Isend error ";
        err(MPI_Isend(sprm.f.data(), sprm.f.size() * sizeof(geom_t), MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, &rsnd[0]), msg, 1);
        err(MPI_Isend(sprm.i.data(), sprm.i.size() * sizeof(llint),  MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, &rsnd[1]), msg, 2);
        err(MPI_Isend(sprm.s.data(), sprm.s.size() * sizeof(short),  MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, &rsnd[2]), msg, 3);
        err(MPI_Isend(sprm.t.data(), sprm.t.size() * sizeof(size_t), MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, &rsnd[3]), msg, 4);
    }
    Wait(piol, rsnd, rrcv);

    if (rank)
        for (size_t i = 0; i < regionSz; i++)
            cpyPrm(i, &rprm, i, prm);
}

/*! Send objects from the current processes to the process one rank lower if
 *  such a process exists. Objects are taken from the start of a vector.
 *  Receiving processes put the objects at the end of their vector.
 *  \tparam T Type of vector
 *  \param[in] piol The PIOL object.
 *  \param[in] regionSz The size of data to send/receive.
 *  \param[in,out] dat Vector to be accessed
 */
void sendLeft(ExSeisPIOL * piol, size_t regionSz, Param * prm)
{
    Param sprm(prm->r, regionSz);
    Param rprm(prm->r, regionSz);

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
            cpyPrm(i, prm, i, &sprm);

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
            cpyPrm(i, &rprm, i+prm->size()-regionSz, prm);
}

void sortP(ExSeisPIOL * piol, File::Param * prm, CompareP comp = nullptr)
{
    size_t lnt = prm->size();
    size_t numRank = piol->comm->getNumRank();
    size_t rank = piol->comm->getRank();
    size_t regionSz = piol->comm->min(lnt) / 4LU;
    size_t edge1 = (rank ? regionSz : 0LU);
    size_t edge2 = (rank != numRank-1 ? regionSz : 0LU);

    std::vector<size_t> t1(lnt);
    std::iota(t1.begin(), t1.end(), 0LU);
    std::sort(t1.begin(), t1.end(), [prm, comp](size_t & a, size_t & b) -> bool { return comp(prm, a, b); });

    File::Param temp1(prm->r, lnt + edge2);
    File::Param temp2(temp1.r, temp1.size());
    File::Param temp3(prm->r, temp1.size());

    for (size_t i = 0; i < lnt; i++)
        cpyPrm(t1[i], prm, i, &temp1);

    while (numRank > 1) //Infinite loop if there is more than one process, otherwise no loop
    {
        temp2 = temp1;
        sendLeft(piol, regionSz, &temp1);

        {
            std::vector<size_t> t1(temp1.size() - edge1);
            std::iota(t1.begin(), t1.end(), edge1);
            std::sort(t1.begin(), t1.end(), [&temp1, comp](size_t & a, size_t & b) -> bool { return comp(&temp1, a, b); });

            for (size_t i = 0; i < t1.size(); i++)
                cpyPrm(t1[i], &temp1, i, &temp3);
            for (size_t i = 0; i < t1.size(); i++)
                cpyPrm(i, &temp3, i+edge1, &temp1);
        }

        sendRight(piol, regionSz, &temp1);

        {
            std::vector<size_t> t1(temp1.size() - edge2);
            std::iota(t1.begin(), t1.end(), 0);
            std::sort(t1.begin(), t1.end(), [&temp1, comp](size_t & a, size_t & b) -> bool { return comp(&temp1, a, b); });

            for (size_t i = 0; i < t1.size(); i++)
                cpyPrm(t1[i], &temp1, i, &temp3);
            for (size_t i = 0; i < t1.size(); i++)
                cpyPrm(i, &temp3, i, &temp1);
        }

        int reduced = 0;
        for (size_t j = 0; j < lnt && !reduced; j++)
            reduced += (getPrm<llint>(j, Meta::gtn, &temp1) != getPrm<llint>(j, Meta::gtn, &temp2));
        int greduced = 1;

        int err = MPI_Allreduce(&reduced, &greduced, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Allreduce error");

        if (!greduced)
            break;
    }

    for (size_t i = 0; i < lnt; i++)
        cpyPrm(i, &temp1, i, prm);
}


std::vector<size_t> sort(ExSeisPIOL * piol, Param * prm, CompareP comp, bool FileOrder)
{
    sortP(piol, prm, comp);

    std::vector<size_t> list(prm->size());
    for (size_t i = 0; i < prm->size(); i++)
        list[i] = getPrm<size_t>(i, Meta::gtn, prm);

    return (FileOrder ? sort(piol, list): list);
}






}}
