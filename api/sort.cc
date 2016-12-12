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
#include "fileops.hh"
#include "ops/sort.hh"
#include "file/file.hh"
#include "share/mpi.hh"

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
 *  \param[in] e1 Structure to access jth parameter of associated Param struct.
 *  \param[in] e2 Structure to access jth parameter of associated Param struct.
 *  \return Return true if e1 is less than e2 in terms of the sort.
 */
bool lessSrcRcv(const Param & e1, const Param & e2)
{
    auto e1sx = getPrm<geom_t>(0U, Meta::xSrc, &e1);
    auto e2sx = getPrm<geom_t>(0U, Meta::xSrc, &e2);

    if (e1sx < e2sx)
        return true;
    else if (e1sx == e2sx)
    {
        auto e1sy = getPrm<geom_t>(0U, Meta::ySrc, &e1);
        auto e2sy = getPrm<geom_t>(0U, Meta::ySrc, &e2);

        if (e1sy < e2sy)
            return true;
        else if (e1sy == e2sy)
        {
            auto e1rx = getPrm<geom_t>(0U, Meta::xRcv, &e1);
            auto e2rx = getPrm<geom_t>(0U, Meta::xRcv, &e2);

            if (e1rx < e2rx)
                return true;
            else if (e1rx == e2rx)
            {
                auto e1ry = getPrm<geom_t>(0U, Meta::yRcv, &e1);
                auto e2ry = getPrm<geom_t>(0U, Meta::yRcv, &e2);

                if (e1ry < e2ry)
                    return true;
                else if (e1ry == e2ry)
                    return (getPrm<llint>(0U, Meta::tn, &e1) < getPrm<llint>(0U, Meta::tn, &e2));
            }
        }
    }
    return false;
}

bool lessSrcOff(const Param & e1, const Param & e2)
{
    auto e1sx = getPrm<geom_t>(0U, Meta::xSrc, &e1);
    auto e2sx = getPrm<geom_t>(0U, Meta::xSrc, &e2);

    if (e1sx < e2sx)
        return true;
    else if (e1sx == e2sx)
    {
        auto e1sy = getPrm<geom_t>(0U, Meta::ySrc, &e1);
        auto e2sy = getPrm<geom_t>(0U, Meta::ySrc, &e2);

        if (e1sy < e2sy)
            return true;
        else if (e1sy == e2sy)
        {
            auto e1rx = getPrm<geom_t>(0U, Meta::xRcv, &e1);
            auto e1ry = getPrm<geom_t>(0U, Meta::yRcv, &e1);
            auto off1 = off(e1sx, e1sy, e1rx, e1ry);

            auto e2rx = getPrm<geom_t>(0U, Meta::xRcv, &e2);
            auto e2ry = getPrm<geom_t>(0U, Meta::yRcv, &e2);
            auto off2 = off(e2sx, e2sy, e2rx, e2ry);

            return (off1 < off2 || (off1 == off2 && getPrm<llint>(0U, Meta::tn, &e1) < getPrm<llint>(0U, Meta::tn, &e2)));
        }
    }
    return false;
}

bool lessRcvOff(const Param & e1, const Param & e2)
{
    auto e1rx = getPrm<geom_t>(0U, Meta::xRcv, &e1);
    auto e2rx = getPrm<geom_t>(0U, Meta::xRcv, &e2);

    if (e1rx < e2rx)
        return true;
    else if (e1rx == e2rx)
    {
        auto e1ry = getPrm<geom_t>(0U, Meta::yRcv, &e1);
        auto e2ry = getPrm<geom_t>(0U, Meta::yRcv, &e2);

        if (e1ry < e2ry)
            return true;
        else if (e1ry == e2ry)
        {
            auto e1sx = getPrm<geom_t>(0U, Meta::xSrc, &e1);
            auto e1sy = getPrm<geom_t>(0U, Meta::ySrc, &e1);
            auto off1 = off(e1sx, e1sy, e1rx, e1ry);

            auto e2sx = getPrm<geom_t>(0U, Meta::xSrc, &e2);
            auto e2sy = getPrm<geom_t>(0U, Meta::ySrc, &e2);
            auto off2 = off(e2sx, e2sy, e2rx, e2ry);

            return (off1 < off2 || (off1 == off2 && getPrm<llint>(0U, Meta::tn, &e1) < getPrm<llint>(0U, Meta::tn, &e2)));
        }
    }
    return false;
}

bool lessLineOff(const Param & e1, const Param & e2)
{
    auto e1il = getPrm<llint>(0U, Meta::il, &e1);
    auto e2il = getPrm<llint>(0U, Meta::il, &e2);

    if (e1il < e2il)
        return true;
    else if (e1il == e2il)
    {
        auto e1xl = getPrm<llint>(0U, Meta::xl, &e1);
        auto e2xl = getPrm<llint>(0U, Meta::xl, &e2);
        if (e1xl < e2xl)
            return true;
        else if (e1xl == e2xl)
        {
            auto e1sx = getPrm<geom_t>(0U, Meta::xSrc, &e1);
            auto e1sy = getPrm<geom_t>(0U, Meta::ySrc, &e1);
            auto e1rx = getPrm<geom_t>(0U, Meta::xRcv, &e1);
            auto e1ry = getPrm<geom_t>(0U, Meta::yRcv, &e1);

            auto e2sx = getPrm<geom_t>(0U, Meta::xSrc, &e2);
            auto e2sy = getPrm<geom_t>(0U, Meta::ySrc, &e2);
            auto e2rx = getPrm<geom_t>(0U, Meta::xRcv, &e2);
            auto e2ry = getPrm<geom_t>(0U, Meta::yRcv, &e2);

            auto off1 = off(e1sx, e1sy, e1rx, e1ry);
            auto off2 = off(e2sx, e2sy, e2rx, e2ry);

            return (off1 < off2 || (off1 == off2 && getPrm<llint>(0U, Meta::tn, &e1) < getPrm<llint>(0U, Meta::tn, &e2)));
        }
    }
    return false;
}

bool lessOffLine(const Param & e1, const Param & e2)
{
    auto e1sx = getPrm<geom_t>(0U, Meta::xSrc, &e1);
    auto e1sy = getPrm<geom_t>(0U, Meta::ySrc, &e1);
    auto e1rx = getPrm<geom_t>(0U, Meta::xRcv, &e1);
    auto e1ry = getPrm<geom_t>(0U, Meta::yRcv, &e1);
    auto off1 = off(e1sx, e1sy, e1rx, e1ry);

    auto e2sx = getPrm<geom_t>(0U, Meta::xSrc, &e2);
    auto e2sy = getPrm<geom_t>(0U, Meta::ySrc, &e2);
    auto e2rx = getPrm<geom_t>(0U, Meta::xRcv, &e2);
    auto e2ry = getPrm<geom_t>(0U, Meta::yRcv, &e2);
    auto off2 = off(e2sx, e2sy, e2rx, e2ry);

    if (off1 < off2)
        return true;
    else if (off1 == off2)
    {
        auto e1il = getPrm<llint>(0U, Meta::il, &e1);
        auto e2il = getPrm<llint>(0U, Meta::il, &e2);
        if (e1il < e2il)
            return true;
        else if (e1il == e2il)
        {
            auto e1xl = getPrm<llint>(0U, Meta::xl, &e1);
            auto e2xl = getPrm<llint>(0U, Meta::xl, &e2);
            return (e1xl < e2xl || (e1xl == e2xl && getPrm<llint>(0U, Meta::tn, &e1) < getPrm<llint>(0U, Meta::tn, &e2)));
        }
    }
    return false;
}

Compare<Param> getComp(SortType type)
{
    switch (type)
    {
        default :
        case SortType::SrcRcv :
            return lessSrcRcv;
        break;
        case SortType::SrcOff :
            return lessSrcOff;
        break;
        case SortType::RcvOff :
            return lessRcvOff;
        break;
        case SortType::LineOff :
            return lessLineOff;
        break;
        case SortType::OffLine :
            return lessOffLine;
        break;
    }
}

std::vector<size_t> sort(ExSeisPIOL * piol, SortType type, size_t nt, size_t offset, Param * prm)
{
    return sort(piol, nt, offset, prm, getComp(type));
}

//TODO: Make this work with SortType type;
bool checkOrder(Interface * src, std::pair<size_t , size_t> dec, SortType type)
{
    auto comp = getComp(type);
    Param prm(dec.second);
    src->readParam(dec.first, dec.second, &prm);
    Param prev(prm.r, 1);
    Param next(prm.r, 1);
    for (size_t i = 0; i < dec.second; i++)
    {
        cpyPrm(i, &prm, 0U, &next);

        if (i && !comp(prev, next))
            return false;
        std::swap(prev, next);
    }
    return true;
}
}}

