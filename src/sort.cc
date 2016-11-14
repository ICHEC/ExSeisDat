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
void Wait(ExSeisPIOL * piol, MPI_Request rcv, MPI_Request snd)
{
    MPI_Status stat;
    int err;
    if (piol->comm->getRank() != piol->comm->getNumRank()-1)
    {
        err = MPI_Wait(&rcv, &stat);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, &stat, "Sort Rcv error");
    }
    if (piol->comm->getRank())
    {
        err = MPI_Wait(&snd, &stat);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, &stat, "Sort Snd error");
    }
}

template <class T>
void sendRight(ExSeisPIOL * piol, size_t regionSz, std::vector<T> & rcv)
{
    size_t rank = piol->comm->getRank();
    size_t cnt = regionSz * sizeof(T);

    MPI_Request rsnd;
    MPI_Request rrcv;

    if (rank)
    {
        int err = MPI_Irecv(rcv.data(), cnt, MPI_CHAR, rank-1, 1, MPI_COMM_WORLD, &rrcv);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Recv error");
    }
    if (rank != piol->comm->getNumRank()-1)
    {
        int err = MPI_Isend(&rcv[rcv.size()-regionSz], cnt, MPI_CHAR, rank+1, 1, MPI_COMM_WORLD, &rsnd);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Send error");
    }
    Wait(piol, rsnd, rrcv);
}

template <class T>
void sendLeft(ExSeisPIOL * piol, size_t regionSz, std::vector<T> & rcv)
{
    size_t rank = piol->comm->getRank();
    size_t cnt = regionSz * sizeof(T);
    MPI_Request rsnd;
    MPI_Request rrcv;

    if (rank)
    {
        int err = MPI_Isend(rcv.data(), cnt, MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, &rsnd);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Send error");
    }
    if (rank != piol->comm->getNumRank()-1)
    {
        int err = MPI_Irecv(&rcv[rcv.size()-regionSz], cnt, MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, &rrcv);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Recv error");
    }
    Wait(piol, rrcv, rsnd);
}

template <class T>
void Sort(ExSeisPIOL * piol, size_t nt, std::vector<T> & thead, Compare<T> comp = nullptr)
{
    size_t numRank = piol->comm->getNumRank();
    size_t rank = piol->comm->getRank();
    size_t regionSz = std::min(nt / (numRank * 4U), std::numeric_limits<int>::max() / sizeof(T));
    size_t edge1 = (rank ? regionSz : 0U);
    size_t edge2 = (rank != numRank-1 ? regionSz : 0U);
    std::vector<T> tSort1(thead.size() + edge2);
    std::vector<T> tSort2(thead.size() + edge2);

    if (comp != nullptr)
        std::sort(thead.begin(), thead.end(), comp);
    else
        std::sort(thead.begin(), thead.end());
    std::copy(thead.begin(), thead.end(), tSort1.begin());

    while (numRank > 1) //Infinite loop if there is more than one process, otherwise no loop
    {
        tSort2 = tSort1;
        sendLeft(piol, regionSz, tSort1);

        if (comp != nullptr)
            std::sort(tSort1.begin() + edge1, tSort1.end(), comp);
        else
            std::sort(tSort1.begin() + edge1, tSort1.end());

        sendRight(piol, regionSz, tSort1);

        if (comp != nullptr)
            std::sort(tSort1.begin(), tSort1.end() - edge2, comp);
        else
            std::sort(tSort1.begin(), tSort1.end() - edge2);

        int reduced = 0;
        for (size_t j = 0; j < thead.size() && !reduced; j++)
            reduced += (tSort1[j] != tSort2[j]);
        int greduced = 1;

        int err = MPI_Allreduce(&reduced, &greduced, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        printErr(piol->log.get(), "", Log::Layer::Ops, err, NULL, "Sort MPI_Allreduce error");

        if (!greduced)
            break;
    }

    for (size_t i = 0; i < thead.size(); i++)
        thead[i] = tSort1[i];
}

std::vector<size_t> Sort(ExSeisPIOL * piol, size_t nt, size_t offset, Param * prm, Compare<AOSParam> comp)
{
    size_t lnt = prm->size();
    auto coords = prm->getAOS();

    Sort(piol, nt, coords, comp);

    std::vector<std::pair<size_t, size_t>> plist(lnt);
    for (size_t i = 0; i < lnt; i++)
    {
        plist[i].first = getPrm(coords[i].j, Meta::tn, coords[i].prm);
        plist[i].second = offset + i;
    }

    Sort(piol, nt, plist);

    std::vector<size_t> list(lnt);
    for (size_t i = 0; i < lnt; i++)
        list[i] = plist[i].second;
    return list;
}


inline geom_t off(geom_t sx, geom_t sy, geom_t rx, geom_t ry)
{
    return (sx-rx)*(sx-rx) + (sy-ry)*(sy-ry);
}

bool lessSrcRcv(const AOSParam & e1, const AOSParam & e2)
{
    size_t j1 = e1.j;
    auto p1 = e1.prm;
    geom_t e1sx = getPrm(j1, Meta::xSrc, p1);
    geom_t e1sy = getPrm(j1, Meta::ySrc, p1);
    geom_t e1rx = getPrm(j1, Meta::xRcv, p1);
    geom_t e1ry = getPrm(j1, Meta::yRcv, p1);
    size_t e1t = getPrm(j1, Meta::tn, p1);

    size_t j2 = e2.j;
    auto p2 = e2.prm;
    geom_t e2sx = getPrm(j2, Meta::xSrc, p2);
    geom_t e2sy = getPrm(j2, Meta::ySrc, p2);
    geom_t e2rx = getPrm(j2, Meta::xRcv, p2);
    geom_t e2ry = getPrm(j2, Meta::yRcv, p2);
    size_t e2t = getPrm(j2, Meta::tn, p2);

    if (e1sx < e2sx)
        return true;
    else if (e1sx == e2sx)
    {
        if (e1sy < e2sy)
            return true;
        else if (e1sy == e2sy)
        {
            if (e1rx < e2rx)
                return true;
            else if (e1rx == e2rx)
            {
                if (e1ry < e2ry)
                    return true;
                else if (e1ry == e2ry)
                    return (e1t < e2t);
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

