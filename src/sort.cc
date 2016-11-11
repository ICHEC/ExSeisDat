#include <algorithm>
#include <iterator>
#include <functional>

#warning temp
#include <iostream>

#include "global.hh"
#include "ops/ops.hh"
#include "file/file.hh"

namespace PIOL { namespace File {

struct PrmEntry
{
    coord_t src;
    coord_t rcv;
    grid_t line;
    size_t tn;

    bool operator==(const PrmEntry & e) const
    {
        return tn == e.tn;
    }
    bool operator!=(const PrmEntry & e) const
    {
        return tn != e.tn;
    }
    bool operator<(const PrmEntry & e) const
    {
        std::cerr << "Critical error\n";
        return src.x < e.src.x && src.y < e.src.y &&
               rcv.x < e.rcv.x && rcv.y < e.rcv.y;
    }
};

#warning Do error handling
template <class T>
void sendRight(ExSeisPIOL * piol, size_t regionSz, std::vector<T> & rcv)
{
    size_t rank = piol->comm->getRank();
    size_t cnt = regionSz * sizeof(T);

    MPI_Status stat;
    if (rank)
        MPI_Recv(rcv.data(), cnt, MPI_CHAR, rank-1, 1, MPI_COMM_WORLD, &stat);
    if (rank != piol->comm->getNumRank()-1)
        MPI_Send(&rcv[rcv.size()-regionSz], cnt, MPI_CHAR, rank+1, 1, MPI_COMM_WORLD);
}

template <class T>
void sendLeft(ExSeisPIOL * piol, size_t regionSz, std::vector<T> & rcv)
{
    size_t rank = piol->comm->getRank();
    size_t cnt = regionSz * sizeof(T);
    MPI_Status stat;
    if (rank)
        MPI_Send(rcv.data(), cnt, MPI_CHAR, rank-1, 0, MPI_COMM_WORLD);
    if (rank != piol->comm->getNumRank()-1)
        MPI_Recv(&rcv[rcv.size()-regionSz], cnt, MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, &stat);
}

template <class T>
using Compare = std::function<bool(const T &, const T &)>;

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

        if (!greduced)
            break;
    }

    for (size_t i = 0; i < thead.size(); i++)
        thead[i] = tSort1[i];
}

std::vector<size_t> Sort(ExSeisPIOL * piol, File::Interface * src, size_t offset, size_t lnt, Compare<PrmEntry> comp)
{
    //TODO: Assumes the decomposition is small enough to allow this allocation
    File::Param prm(lnt);
    std::vector<PrmEntry> coords(lnt);

    src->readParam(offset, lnt, &prm);
    for (size_t i = 0; i < lnt; i++)
    {
        coords[i].src.x = getPrm(i, Meta::xSrc, &prm);
        coords[i].src.y = getPrm(i, Meta::ySrc, &prm);
        coords[i].rcv.x = getPrm(i, Meta::xRcv, &prm);
        coords[i].rcv.y = getPrm(i, Meta::yRcv, &prm);
        coords[i].line.il = getPrm(i, Meta::il, &prm);
        coords[i].line.xl = getPrm(i, Meta::xl, &prm);
        coords[i].tn = offset + i;
    }

    Sort(piol, src->readNt(), coords, comp);
    std::vector<std::pair<size_t, size_t>> plist(lnt);
    for (size_t i = 0; i < lnt; i++)
    {
        plist[i].first = coords[i].tn;
        plist[i].second = offset + i;
    }

    Sort(piol, src->readNt(), plist);

    std::vector<size_t> list(lnt);
    for (size_t i = 0; i < lnt; i++)
        list[i] = plist[i].second;
    return list;
}

inline geom_t off(geom_t sx, geom_t sy, geom_t rx, geom_t ry)
{
    return (sx-rx)*(sx-rx) + (sx-rx)*(sx-rx);
}

inline geom_t off(const PrmEntry & e)
{
    return off(e.src.x, e.src.y, e.rcv.x, e.rcv.y);
}

std::vector<size_t> Sort(ExSeisPIOL * piol, SortType type, File::Interface * src, size_t offset, size_t lnt)
{
    Compare<PrmEntry> comp = nullptr;
    switch (type)
    {
        default :
        case SortType::SrcRcv :
        std::cout << "sort src rcv\n";
        comp = [] (const PrmEntry & e1, const PrmEntry & e2) -> bool
            {
                return (e1.src.x == e2.src.x && e1.tn < e2.tn) || e1.src.x < e2.src.x;

/*            if (e1.src.x < e2.src.x)
                return true;
            else if (e1.src.x == e2.src.x)
            {
                if (e1.src.y < e2.src.y)
                    return true;
                else if (e1.src.y == e2.src.y)
                {
                    if (e1.rcv.x < e2.rcv.x)
                        return true;
                    else if (e1.rcv.x == e2.rcv.x)
                    {
                        if (e1.rcv.y < e2.rcv.y)
                            return true;
                        else if (e1.rcv.y == e2.rcv.y)
                            return (e1.tn < e2.tn);
                    }
                }
            }
            return false;*/
            };
        break;
        case SortType::OffsetLine :
        comp = [] (const PrmEntry & e1, const PrmEntry & e2) -> bool
        {
            return false;
        };
        break;
        case SortType::CmpSrc :
        comp = [] (const PrmEntry & e1, const PrmEntry & e2) -> bool
        {
            return false;
        };
        break;
    }
    return Sort(piol, src, offset, lnt, comp);
}
}}

