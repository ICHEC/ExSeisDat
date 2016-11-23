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

