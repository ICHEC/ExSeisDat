////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date November 2016
/// @brief The Sort Operation
/// @details The algorithm used is a nearest neighbour approach where at each
///          iteration the lowest valued metadata entries are moved to adjacent
///          processes with a lower rank and a sort is performed. After the sort
///          the highest entries are moved again to the process with a higher
///          rank. If each process has the same traces it started off with, the
///          sort is complete.
////////////////////////////////////////////////////////////////////////////////

#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/param_utils.hh"

#include "ExSeisDat/PIOL/mpi_utils.hh"
#include "ExSeisDat/PIOL/operations/sort.hh"
#include "ExSeisDat/PIOL/share/api.hh"
#include "ExSeisDat/PIOL/typedefs.h"

#include <algorithm>
#include <numeric>

namespace PIOL {

/*! Calculate the square of the hypotenuse
 *  @param[in] sx The source x coordinate
 *  @param[in] sy The source y coordinate
 *  @param[in] rx The receiver x coordinate
 *  @param[in] ry The receiver y coordinate
 *  @return square of the hypotenuse
 */
inline geom_t off(geom_t sx, geom_t sy, geom_t rx, geom_t ry)
{
    return (sx - rx) * (sx - rx) + (sy - ry) * (sy - ry);
}

/*! For sorting by Src X, Src Y, Rcv X, Rcv Y.
 *  @param[in] prm The parameter structure
 *  @param[in] i   Structure to access 0th parameter of associated Param struct.
 *  @param[in] j   Structure to access 0th parameter of associated Param struct.
 *  @return Return true if entry \c i of \p prm is less than entry \c j in terms
 *          of the sort.
 */
static bool lessSrcRcv(const Param* prm, const size_t i, const size_t j)
{
    auto e1sx = param_utils::getPrm<geom_t>(i, PIOL_META_xSrc, prm);
    auto e2sx = param_utils::getPrm<geom_t>(j, PIOL_META_xSrc, prm);

    if (e1sx < e2sx) {
        return true;
    }
    else if (e1sx == e2sx) {
        auto e1sy = param_utils::getPrm<geom_t>(i, PIOL_META_ySrc, prm);
        auto e2sy = param_utils::getPrm<geom_t>(j, PIOL_META_ySrc, prm);

        if (e1sy < e2sy) {
            return true;
        }
        else if (e1sy == e2sy) {
            auto e1rx = param_utils::getPrm<geom_t>(i, PIOL_META_xRcv, prm);
            auto e2rx = param_utils::getPrm<geom_t>(j, PIOL_META_xRcv, prm);

            if (e1rx < e2rx) {
                return true;
            }
            else if (e1rx == e2rx) {
                auto e1ry = param_utils::getPrm<geom_t>(i, PIOL_META_yRcv, prm);
                auto e2ry = param_utils::getPrm<geom_t>(j, PIOL_META_yRcv, prm);

                if (e1ry < e2ry) {
                    return true;
                }
                else if (e1ry == e2ry) {
                    return (
                      param_utils::getPrm<llint>(i, PIOL_META_ltn, prm)
                      < param_utils::getPrm<llint>(j, PIOL_META_ltn, prm));
                }
            }
        }
    }

    return false;
}

/*! For sorting by Src X, Src Y and Offset.
 *  @tparam    CalcOff If true, calculate the offset, otherwise read the offset
 *                     from the header
 *  @param[in] prm     The parameter structure
 *  @param[in] i       Structure to access 0th parameter of associated Param
 *                     struct.
 *  @param[in] j       Structure to access 0th parameter of associated Param
 *                     struct.
 *  @return Return true if entry \c i of \p prm is less than entry \c j in terms
 *          of the sort.
 */
template<bool CalcOff>
static bool lessSrcOff(const Param* prm, const size_t i, const size_t j)
{
    auto e1sx = param_utils::getPrm<geom_t>(i, PIOL_META_xSrc, prm);
    auto e2sx = param_utils::getPrm<geom_t>(j, PIOL_META_xSrc, prm);

    if (e1sx < e2sx) {
        return true;
    }
    else if (e1sx == e2sx) {
        auto e1sy = param_utils::getPrm<geom_t>(i, PIOL_META_ySrc, prm);
        auto e2sy = param_utils::getPrm<geom_t>(j, PIOL_META_ySrc, prm);

        if (e1sy < e2sy) {
            return true;
        }
        else if (e1sy == e2sy) {
            auto e1rx = param_utils::getPrm<geom_t>(i, PIOL_META_xRcv, prm);
            auto e1ry = param_utils::getPrm<geom_t>(i, PIOL_META_yRcv, prm);
            auto off1 =
              (CalcOff ? off(e1sx, e1sy, e1rx, e1ry) :
                         param_utils::getPrm<size_t>(i, PIOL_META_Offset, prm));

            auto e2rx = param_utils::getPrm<geom_t>(j, PIOL_META_xRcv, prm);
            auto e2ry = param_utils::getPrm<geom_t>(j, PIOL_META_yRcv, prm);
            auto off2 =
              (CalcOff ? off(e2sx, e2sy, e2rx, e2ry) :
                         param_utils::getPrm<size_t>(j, PIOL_META_Offset, prm));

            return (
              off1 < off2
              || (off1 == off2
                  && param_utils::getPrm<llint>(i, PIOL_META_ltn, prm)
                       < param_utils::getPrm<llint>(j, PIOL_META_ltn, prm)));
        }
    }
    return false;
}

/*! For sorting by Rcv X, Rcv Y and Offset.
 *  @tparam    CalcOff If true, calculate the offset, otherwise read the offset
 *                     from the header
 *  @param[in] prm     The parameter structure
 *  @param[in] i       Structure to access 0th parameter of associated Param
 *                     struct.
 *  @param[in] j       Structure to access 0th parameter of associated Param
 *                     struct.
 *  @return Return true if entry \c i of \p prm is less than entry \c j in terms
 *          of the sort.
 */
template<bool CalcOff>
static bool lessRcvOff(const Param* prm, const size_t i, const size_t j)
{
    auto e1rx = param_utils::getPrm<geom_t>(i, PIOL_META_xRcv, prm);
    auto e2rx = param_utils::getPrm<geom_t>(j, PIOL_META_xRcv, prm);

    if (e1rx < e2rx) {
        return true;
    }
    else if (e1rx == e2rx) {
        auto e1ry = param_utils::getPrm<geom_t>(i, PIOL_META_yRcv, prm);
        auto e2ry = param_utils::getPrm<geom_t>(j, PIOL_META_yRcv, prm);

        if (e1ry < e2ry) {
            return true;
        }
        else if (e1ry == e2ry) {
            auto e1sx = param_utils::getPrm<geom_t>(i, PIOL_META_xSrc, prm);
            auto e1sy = param_utils::getPrm<geom_t>(i, PIOL_META_ySrc, prm);
            auto off1 =
              (CalcOff ? off(e1sx, e1sy, e1rx, e1ry) :
                         param_utils::getPrm<size_t>(i, PIOL_META_Offset, prm));

            auto e2sx = param_utils::getPrm<geom_t>(j, PIOL_META_xSrc, prm);
            auto e2sy = param_utils::getPrm<geom_t>(j, PIOL_META_ySrc, prm);
            auto off2 =
              (CalcOff ? off(e2sx, e2sy, e2rx, e2ry) :
                         param_utils::getPrm<size_t>(j, PIOL_META_Offset, prm));

            return (
              off1 < off2
              || (off1 == off2
                  && param_utils::getPrm<llint>(i, PIOL_META_ltn, prm)
                       < param_utils::getPrm<llint>(j, PIOL_META_ltn, prm)));
        }
    }
    return false;
}

/*! For sorting by Inline, Crossline and Offset.
 *  @tparam CalcOff If true, calculate the offset, otherwise read the offset
 *                  from the header
 *  @param[in] prm The parameter structure
 *  @param[in] i   Structure to access 0th parameter of associated Param struct.
 *  @param[in] j   Structure to access 0th parameter of associated Param struct.
 *  @return Return true if entry \c i of \p prm is less than entry \c j in terms
 *          of the sort.
 */
template<bool CalcOff>
static bool lessLineOff(const Param* prm, const size_t i, const size_t j)
{
    auto e1il = param_utils::getPrm<llint>(i, PIOL_META_il, prm);
    auto e2il = param_utils::getPrm<llint>(j, PIOL_META_il, prm);

    if (e1il < e2il) {
        return true;
    }
    else if (e1il == e2il) {
        auto e1xl = param_utils::getPrm<llint>(i, PIOL_META_xl, prm);
        auto e2xl = param_utils::getPrm<llint>(j, PIOL_META_xl, prm);
        if (e1xl < e2xl) {
            return true;
        }
        else if (e1xl == e2xl) {
            auto e1sx = param_utils::getPrm<geom_t>(i, PIOL_META_xSrc, prm);
            auto e1sy = param_utils::getPrm<geom_t>(i, PIOL_META_ySrc, prm);
            auto e1rx = param_utils::getPrm<geom_t>(i, PIOL_META_xRcv, prm);
            auto e1ry = param_utils::getPrm<geom_t>(i, PIOL_META_yRcv, prm);

            auto e2sx = param_utils::getPrm<geom_t>(j, PIOL_META_xSrc, prm);
            auto e2sy = param_utils::getPrm<geom_t>(j, PIOL_META_ySrc, prm);
            auto e2rx = param_utils::getPrm<geom_t>(j, PIOL_META_xRcv, prm);
            auto e2ry = param_utils::getPrm<geom_t>(j, PIOL_META_yRcv, prm);

            auto off1 =
              (CalcOff ? off(e1sx, e1sy, e1rx, e1ry) :
                         param_utils::getPrm<size_t>(i, PIOL_META_Offset, prm));
            auto off2 =
              (CalcOff ? off(e2sx, e2sy, e2rx, e2ry) :
                         param_utils::getPrm<size_t>(j, PIOL_META_Offset, prm));
            return (
              off1 < off2
              || (off1 == off2
                  && param_utils::getPrm<llint>(i, PIOL_META_ltn, prm)
                       < param_utils::getPrm<llint>(j, PIOL_META_ltn, prm)));
        }
    }
    return false;
}

/*! For sorting by Offset, Inline and Crossline.
 *  @tparam    CalcOff If true, calculate the offset, otherwise read the offset
 *                     from the header
 *  @param[in] prm     The parameter structure
 *  @param[in] i       Structure to access 0th parameter of associated Param
 *                     struct.
 *  @param[in] j       Structure to access 0th parameter of associated Param
 *                     struct.
 *  @return Return true if entry \c i of \p prm is less than entry \c j in terms
 *          of the sort.
 */
template<bool CalcOff>
static bool lessOffLine(const Param* prm, const size_t i, const size_t j)
{
    auto e1sx = param_utils::getPrm<geom_t>(i, PIOL_META_xSrc, prm);
    auto e1sy = param_utils::getPrm<geom_t>(i, PIOL_META_ySrc, prm);
    auto e1rx = param_utils::getPrm<geom_t>(i, PIOL_META_xRcv, prm);
    auto e1ry = param_utils::getPrm<geom_t>(i, PIOL_META_yRcv, prm);
    auto off1 =
      (CalcOff ? off(e1sx, e1sy, e1rx, e1ry) :
                 param_utils::getPrm<size_t>(i, PIOL_META_Offset, prm));

    auto e2sx = param_utils::getPrm<geom_t>(j, PIOL_META_xSrc, prm);
    auto e2sy = param_utils::getPrm<geom_t>(j, PIOL_META_ySrc, prm);
    auto e2rx = param_utils::getPrm<geom_t>(j, PIOL_META_xRcv, prm);
    auto e2ry = param_utils::getPrm<geom_t>(j, PIOL_META_yRcv, prm);
    auto off2 =
      (CalcOff ? off(e2sx, e2sy, e2rx, e2ry) :
                 param_utils::getPrm<size_t>(j, PIOL_META_Offset, prm));

    if (off1 < off2) {
        return true;
    }
    else if (off1 == off2) {
        auto e1il = param_utils::getPrm<llint>(i, PIOL_META_il, prm);
        auto e2il = param_utils::getPrm<llint>(j, PIOL_META_il, prm);
        if (e1il < e2il) {
            return true;
        }
        else if (e1il == e2il) {
            auto e1xl = param_utils::getPrm<llint>(i, PIOL_META_xl, prm);
            auto e2xl = param_utils::getPrm<llint>(j, PIOL_META_xl, prm);
            return (
              e1xl < e2xl
              || (e1xl == e2xl
                  && param_utils::getPrm<llint>(i, PIOL_META_ltn, prm)
                       < param_utils::getPrm<llint>(j, PIOL_META_ltn, prm)));
        }
    }
    return false;
}

CompareP getComp(SortType type)
{
    switch (type) {
        default:
        case PIOL_SORTTYPE_SrcRcv:
            return lessSrcRcv;
            break;
        case PIOL_SORTTYPE_SrcOff:
            return lessSrcOff<true>;
            break;
        case PIOL_SORTTYPE_SrcROff:
            return lessSrcOff<false>;
            break;
        case PIOL_SORTTYPE_RcvOff:
            return lessRcvOff<true>;
            break;
        case PIOL_SORTTYPE_RcvROff:
            return lessRcvOff<false>;
            break;
        case PIOL_SORTTYPE_LineOff:
            return lessLineOff<true>;
            break;
        case PIOL_SORTTYPE_LineROff:
            return lessLineOff<false>;
            break;
        case PIOL_SORTTYPE_OffLine:
            return lessOffLine<true>;
            break;
        case PIOL_SORTTYPE_ROffLine:
            return lessOffLine<false>;
            break;
    }
}

std::vector<size_t> sort(ExSeisPIOL* piol, SortType type, Param* prm)
{
    return sort(piol, prm, getComp(type));
}

bool checkOrder(ReadInterface* src, Range dec, SortType type)
{
    auto comp = getComp(type);
    Param prm(dec.size);

    src->readParam(dec.offset, dec.size, &prm);

    for (size_t i = 1; i < dec.size; i++) {
        if (!comp(&prm, i - 1, i)) {
            return false;
        }
    }

    return true;
}

/**************************** Core Implementation *****************************/

/*! Wait on two requests to finish. The largest and smallest rank only wait on
 *  one request.
 *  @param[in] piol The PIOL object.
 *  @param[in] req1 The request that all processes except rank of numRank-1 wait
 *                  on.
 *  @param[in] req2 The request that all processes except rank of 0 wait on..
 */
void Wait(
  ExSeisPIOL* piol,
  std::vector<MPI_Request> req1,
  std::vector<MPI_Request> req2)
{
    MPI_Status stat;
    int err;
    if (piol->comm->getRank() != piol->comm->getNumRank() - 1) {
        for (size_t i = 0; i < req1.size(); i++) {
            err = MPI_Wait(&req1[i], &stat);
            MPI_utils::printErr(
              piol->log.get(), "", Logger::Layer::Ops, err, &stat,
              "Sort Rcv error");
        }
    }

    if (piol->comm->getRank()) {
        for (size_t i = 0; i < req2.size(); i++) {
            err = MPI_Wait(&req2[i], &stat);
            MPI_utils::printErr(
              piol->log.get(), "", Logger::Layer::Ops, err, &stat,
              "Sort Snd error");
        }
    }
}

/*! Send objects from the current processes to the process one rank higher if
 *  such a process exists. Objects are taken from the end of a vector.
 *  Receiving processes put the objects at the start of their vector.
 *  @tparam T Type of vector
 *  @param[in] piol The PIOL object.
 *  @param[in] regionSz The size of data to send/receive.
 *  @param[in,out] dat Vector to be accessed
 */
template<class T>
void sendRight(ExSeisPIOL* piol, size_t regionSz, std::vector<T>& dat)
{
    size_t rank = piol->comm->getRank();
    size_t cnt  = regionSz * sizeof(T);
    std::vector<MPI_Request> rsnd(1);
    std::vector<MPI_Request> rrcv(1);

    // TODO: Move to the communication layer?
    if (rank) {
        int err = MPI_Irecv(
          dat.data(), cnt, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD, &rrcv[0]);
        MPI_utils::printErr(
          piol->log.get(), "", Logger::Layer::Ops, err, NULL,
          "Sort MPI_Recv error");
    }

    if (rank != piol->comm->getNumRank() - 1) {
        int err = MPI_Isend(
          &dat[dat.size() - regionSz], cnt, MPI_CHAR, rank + 1, 1,
          MPI_COMM_WORLD, &rsnd[0]);
        MPI_utils::printErr(
          piol->log.get(), "", Logger::Layer::Ops, err, NULL,
          "Sort MPI_Send error");
    }

    Wait(piol, rsnd, rrcv);
}

/*! Send objects from the current processes to the process one rank lower if
 *  such a process exists. Objects are taken from the start of a vector.
 *  Receiving processes put the objects at the end of their vector.
 *  @tparam T Type of vector
 *  @param[in] piol The PIOL object.
 *  @param[in] regionSz The size of data to send/receive.
 *  @param[in,out] dat Vector to be accessed
 */
template<class T>
void sendLeft(ExSeisPIOL* piol, size_t regionSz, std::vector<T>& dat)
{
    size_t rank = piol->comm->getRank();
    size_t cnt  = regionSz * sizeof(T);
    std::vector<MPI_Request> rsnd(1);
    std::vector<MPI_Request> rrcv(1);

    if (rank) {
        int err = MPI_Isend(
          dat.data(), cnt, MPI_CHAR, rank - 1, 0, MPI_COMM_WORLD, &rsnd[0]);
        MPI_utils::printErr(
          piol->log.get(), "", Logger::Layer::Ops, err, NULL,
          "Sort MPI_Send error");
    }

    if (rank != piol->comm->getNumRank() - 1) {
        int err = MPI_Irecv(
          &dat[dat.size() - regionSz], cnt, MPI_CHAR, rank + 1, 0,
          MPI_COMM_WORLD, &rrcv[0]);
        MPI_utils::printErr(
          piol->log.get(), "", Logger::Layer::Ops, err, NULL,
          "Sort MPI_Recv error");
    }

    Wait(piol, rrcv, rsnd);
}

/*! Function to sort a given vector by a nearest neighbour approach.
 *  @tparam T Type of vector
 *  @param[in] piol The PIOL object.
 *  @param[in] regionSz The size of the region which will be shared
 *  @param[in, out] temp1 Temporary vector which is the dat.size()+regionSz.
 *  @param[in,out] dat The vector to sort
 *  @param[in] comp The function to use for less-than comparisons between
 *                  objects in the vector.
 */
template<class T>
void sort(
  ExSeisPIOL* piol,
  size_t regionSz,
  std::vector<T>& temp1,
  std::vector<T>& dat,
  Compare<T> comp = nullptr)
{
    size_t lnt     = dat.size();
    size_t numRank = piol->comm->getNumRank();
    size_t rank    = piol->comm->getRank();
    size_t edge1   = (rank ? regionSz : 0LU);
    size_t edge2   = (rank != numRank - 1LU ? regionSz : 0LU);

    if (comp != nullptr) {
        std::sort(
          dat.begin(), dat.begin() + lnt,
          [comp](auto& a, auto& b) -> bool { return comp(a, b); });
    }
    else {
        std::sort(dat.begin(), dat.begin() + lnt);
    }

    std::copy(dat.begin(), dat.end(), temp1.begin());
    std::vector<T> temp2;
    // Infinite loop if there is more than one process, otherwise no loop
    while (numRank > 1) {
        temp2 = temp1;
        sendLeft(piol, regionSz, temp1);

        if (comp != nullptr) {
            std::sort(
              temp1.begin() + edge1, temp1.end(),
              [comp](auto& a, auto& b) -> bool { return comp(a, b); });
        }
        else {
            // default pair sorting is first then second
            std::sort(temp1.begin() + edge1, temp1.end());
        }

        sendRight(piol, regionSz, temp1);

        if (comp != nullptr) {
            std::sort(
              temp1.begin(), temp1.end() - edge2,
              [comp](auto& a, auto& b) -> bool { return comp(a, b); });
        }
        else {
            std::sort(temp1.begin(), temp1.end() - edge2);
        }

        int reduced = 0;
        for (size_t j = 0; j < lnt && !reduced; j++) {
            reduced += (temp1[j] != temp2[j]);
        }
        int greduced = 1;

        int err = MPI_Allreduce(
          &reduced, &greduced, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        MPI_utils::printErr(
          piol->log.get(), "", Logger::Layer::Ops, err, NULL,
          "Sort MPI_Allreduce error");

        if (!greduced) break;
    }

    for (size_t i = 0; i < lnt; i++) {
        dat[i] = temp1[i];
    }
}

/*! Parallel sort a list. Local vector is part of the entire list.
 *  @param[in] piol The PIOL object.
 *  @param[in] list The local vector
 *  @return Return a new sorted vector
 */
std::vector<size_t> sort(ExSeisPIOL* piol, std::vector<size_t> list)
{
    const size_t lnt      = list.size();
    const size_t regionSz = piol->comm->min(lnt) / 4LU;
    const size_t edge2 =
      (piol->comm->getRank() != piol->comm->getNumRank() - 1 ? regionSz : 0LU);
    const size_t offset = piol->comm->offset(lnt);

    std::vector<std::pair<size_t, size_t>> plist(lnt);
    for (size_t i = 0; i < lnt; i++) {
        plist[i].first  = list[i];
        plist[i].second = offset + i;
    }

    {
        std::vector<std::pair<size_t, size_t>> temp1(lnt + edge2);

// Work-around for bug in the intel compiler (intel/2016-u3) on Fionn with
// std::pair less-than operator
#if defined(__INTEL_COMPILER)
        Compare<std::pair<size_t, size_t>> check =
          [](
            const std::pair<size_t, size_t>& e1,
            const std::pair<size_t, size_t>& e2) -> bool {
            return (
              e1.first < e2.first
              || (e1.first == e2.first && e1.second < e2.second));
        };
        sort(piol, regionSz, temp1, plist, check);
#else
        sort(piol, regionSz, temp1, plist);
#endif
    }

    for (size_t i = 0; i < lnt; i++) {
        list[i] = plist[i].second;
    }

    return list;
}

/*! Send objects from the current processes to the process one rank higher if
 *  such a process exists. Objects are taken from the end of a vector.
 *  Receiving processes put the objects at the start of their vector.
 *  @param[in]     piol     The PIOL object.
 *  @param[in]     regionSz The size of data to send/receive.
 *  @param[in,out] prm      The parameter structure to send/receive
 */
void sendRight(ExSeisPIOL* piol, size_t regionSz, Param* prm)
{
    Param sprm(prm->r, regionSz);
    Param rprm(prm->r, regionSz);

    size_t rank = piol->comm->getRank();
    std::vector<MPI_Request> rsnd(4);
    std::vector<MPI_Request> rrcv(4);

    Logger* log = piol->log.get();
    auto err    = [log](int errc, std::string msg, size_t i) -> void {
        MPI_utils::printErr(
          log, "", Logger::Layer::Ops, errc, NULL, msg + std::to_string(i));
    };

    if (rank) {
        std::string msg = "Sort right MPI_Irecv error ";
        err(
          MPI_Irecv(
            rprm.f.data(), rprm.f.size() * sizeof(geom_t), MPI_CHAR, rank - 1,
            0, MPI_COMM_WORLD, &rrcv[0]),
          msg, 1);
        err(
          MPI_Irecv(
            rprm.i.data(), rprm.i.size() * sizeof(llint), MPI_CHAR, rank - 1, 0,
            MPI_COMM_WORLD, &rrcv[1]),
          msg, 2);
        err(
          MPI_Irecv(
            rprm.s.data(), rprm.s.size() * sizeof(short), MPI_CHAR, rank - 1, 0,
            MPI_COMM_WORLD, &rrcv[2]),
          msg, 3);
        err(
          MPI_Irecv(
            rprm.t.data(), rprm.t.size() * sizeof(size_t), MPI_CHAR, rank - 1,
            0, MPI_COMM_WORLD, &rrcv[3]),
          msg, 4);
    }

    if (rank != piol->comm->getNumRank() - 1) {
        for (size_t i = 0; i < regionSz; i++) {
            param_utils::cpyPrm(i + prm->size() - regionSz, prm, i, &sprm);
        }

        std::string msg = "Sort right MPI_Isend error ";
        err(
          MPI_Isend(
            sprm.f.data(), sprm.f.size() * sizeof(geom_t), MPI_CHAR, rank + 1,
            0, MPI_COMM_WORLD, &rsnd[0]),
          msg, 1);
        err(
          MPI_Isend(
            sprm.i.data(), sprm.i.size() * sizeof(llint), MPI_CHAR, rank + 1, 0,
            MPI_COMM_WORLD, &rsnd[1]),
          msg, 2);
        err(
          MPI_Isend(
            sprm.s.data(), sprm.s.size() * sizeof(short), MPI_CHAR, rank + 1, 0,
            MPI_COMM_WORLD, &rsnd[2]),
          msg, 3);
        err(
          MPI_Isend(
            sprm.t.data(), sprm.t.size() * sizeof(size_t), MPI_CHAR, rank + 1,
            0, MPI_COMM_WORLD, &rsnd[3]),
          msg, 4);
    }

    Wait(piol, rsnd, rrcv);

    if (rank) {
        for (size_t i = 0; i < regionSz; i++) {
            param_utils::cpyPrm(i, &rprm, i, prm);
        }
    }
}

/*! Send objects from the current processes to the process one rank lower if
 *  such a process exists. Objects are taken from the start of a vector.
 *  Receiving processes put the objects at the end of their vector.
 *  @param[in]     piol     The PIOL object.
 *  @param[in]     regionSz The size of data to send/receive.
 *  @param[in,out] prm      The parameter structure to send/receive
 */
void sendLeft(ExSeisPIOL* piol, size_t regionSz, Param* prm)
{
    Param sprm(prm->r, regionSz);
    Param rprm(prm->r, regionSz);

    size_t rank = piol->comm->getRank();
    std::vector<MPI_Request> rsnd(4);
    std::vector<MPI_Request> rrcv(4);

    Logger* log = piol->log.get();
    auto err    = [log](int errc, std::string msg, size_t i) -> void {
        MPI_utils::printErr(
          log, "", Logger::Layer::Ops, errc, NULL, msg + std::to_string(i));
    };

    if (rank) {
        for (size_t i = 0; i < regionSz; i++) {
            param_utils::cpyPrm(i, prm, i, &sprm);
        }

        std::string msg = "Sort left MPI_Irecv error ";
        err(
          MPI_Isend(
            sprm.f.data(), sprm.f.size() * sizeof(geom_t), MPI_CHAR, rank - 1,
            1, MPI_COMM_WORLD, &rsnd[0]),
          msg, 1);
        err(
          MPI_Isend(
            sprm.i.data(), sprm.i.size() * sizeof(llint), MPI_CHAR, rank - 1, 1,
            MPI_COMM_WORLD, &rsnd[1]),
          msg, 2);
        err(
          MPI_Isend(
            sprm.s.data(), sprm.s.size() * sizeof(short), MPI_CHAR, rank - 1, 1,
            MPI_COMM_WORLD, &rsnd[2]),
          msg, 3);
        err(
          MPI_Isend(
            sprm.t.data(), sprm.t.size() * sizeof(size_t), MPI_CHAR, rank - 1,
            1, MPI_COMM_WORLD, &rsnd[3]),
          msg, 4);
    }

    if (rank != piol->comm->getNumRank() - 1) {
        std::string msg = "Sort left MPI_Isend error ";
        err(
          MPI_Irecv(
            rprm.f.data(), rprm.f.size() * sizeof(geom_t), MPI_CHAR, rank + 1,
            1, MPI_COMM_WORLD, &rrcv[0]),
          msg, 1);
        err(
          MPI_Irecv(
            rprm.i.data(), rprm.i.size() * sizeof(llint), MPI_CHAR, rank + 1, 1,
            MPI_COMM_WORLD, &rrcv[1]),
          msg, 2);
        err(
          MPI_Irecv(
            rprm.s.data(), rprm.s.size() * sizeof(short), MPI_CHAR, rank + 1, 1,
            MPI_COMM_WORLD, &rrcv[2]),
          msg, 3);
        err(
          MPI_Irecv(
            rprm.t.data(), rprm.t.size() * sizeof(size_t), MPI_CHAR, rank + 1,
            1, MPI_COMM_WORLD, &rrcv[3]),
          msg, 4);
    }

    Wait(piol, rrcv, rsnd);

    if (rank != piol->comm->getNumRank() - 1) {
        for (size_t i = 0; i < regionSz; i++) {
            param_utils::cpyPrm(i, &rprm, i + prm->size() - regionSz, prm);
        }
    }
}

/// Sort the parameter structure across all processes
/// @param[in] piol The ExSeisPIOL object
/// @param[in] prm  The parameter structure
/// @param[in] comp The comparison operator to sort the headers by.
void sortP(ExSeisPIOL* piol, Param* prm, CompareP comp = nullptr)
{
    size_t lnt      = prm->size();
    size_t numRank  = piol->comm->getNumRank();
    size_t rank     = piol->comm->getRank();
    size_t regionSz = piol->comm->min(lnt) / 4LU;
    size_t edge1    = (rank ? regionSz : 0LU);
    size_t edge2    = (rank != numRank - 1 ? regionSz : 0LU);

    std::vector<size_t> t1(lnt);
    std::iota(t1.begin(), t1.end(), 0LU);
    std::sort(t1.begin(), t1.end(), [prm, comp](size_t& a, size_t& b) -> bool {
        return comp(prm, a, b);
    });

    Param temp1(prm->r, lnt + edge2);
    Param temp2(temp1.r, temp1.size());
    Param temp3(prm->r, temp1.size());

    for (size_t i = 0; i < lnt; i++) {
        param_utils::cpyPrm(t1[i], prm, i, &temp1);
    }

    // Infinite loop if there is more than one process, otherwise no loop
    while (numRank > 1) {
        temp2 = temp1;
        sendLeft(piol, regionSz, &temp1);

        {
            std::vector<size_t> t1(temp1.size() - edge1);
            std::iota(t1.begin(), t1.end(), edge1);
            std::sort(
              t1.begin(), t1.end(),
              [&temp1, comp](size_t& a, size_t& b) -> bool {
                  return comp(&temp1, a, b);
              });

            for (size_t i = 0; i < t1.size(); i++) {
                param_utils::cpyPrm(t1[i], &temp1, i, &temp3);
            }

            for (size_t i = 0; i < t1.size(); i++) {
                param_utils::cpyPrm(i, &temp3, i + edge1, &temp1);
            }
        }

        sendRight(piol, regionSz, &temp1);

        {
            std::vector<size_t> t1(temp1.size() - edge2);
            std::iota(t1.begin(), t1.end(), 0);
            std::sort(
              t1.begin(), t1.end(),
              [&temp1, comp](size_t& a, size_t& b) -> bool {
                  return comp(&temp1, a, b);
              });

            for (size_t i = 0; i < t1.size(); i++) {
                param_utils::cpyPrm(t1[i], &temp1, i, &temp3);
            }

            for (size_t i = 0; i < t1.size(); i++) {
                param_utils::cpyPrm(i, &temp3, i, &temp1);
            }
        }

        int reduced = 0;
        for (size_t j = 0; j < lnt && !reduced; j++) {
            reduced +=
              (param_utils::getPrm<llint>(j, PIOL_META_gtn, &temp1)
               != param_utils::getPrm<llint>(j, PIOL_META_gtn, &temp2));
        }
        int greduced = 1;

        int err = MPI_Allreduce(
          &reduced, &greduced, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

        MPI_utils::printErr(
          piol->log.get(), "", Logger::Layer::Ops, err, NULL,
          "Sort MPI_Allreduce error");

        if (!greduced) break;
    }

    for (size_t i = 0; i < lnt; i++) {
        param_utils::cpyPrm(i, &temp1, i, prm);
    }
}

std::vector<size_t> sort(
  ExSeisPIOL* piol, Param* prm, CompareP comp, bool FileOrder)
{
    sortP(piol, prm, comp);

    std::vector<size_t> list(prm->size());
    for (size_t i = 0; i < prm->size(); i++)
        list[i] = param_utils::getPrm<size_t>(i, PIOL_META_gtn, prm);

    return (FileOrder ? sort(piol, list) : list);
}

}  // namespace PIOL
