#include <assert.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "cppfileapi.hh"
#include "sglobal.hh"
#include "share/smpi.hh"
using namespace PIOL;
using namespace File;

template <class T>
using cvec = const std::vector<T>;

template <class T>
using vec = std::vector<T>;

struct entry
{
    coord_t src;
    coord_t rcv;
    size_t tn;
    bool operator==(const entry & e) const
    {
        return tn == e.tn;
    }
    bool operator!=(const entry & e) const
    {
        return tn != e.tn;
    }

};

void getCoords(File::Interface * file, size_t offset, vec<entry> & coords)
{
    size_t sz = coords.size();
    File::Param prm(sz);
    file->readParam(offset, sz, &prm);
    for (size_t i = 0; i < sz; i++)
    {
        coords[i].src.x = getPrm(i, Meta::xSrc, &prm);
        coords[i].src.y = getPrm(i, Meta::ySrc, &prm);
        coords[i].rcv.x = getPrm(i, Meta::xRcv, &prm);
        coords[i].rcv.y = getPrm(i, Meta::yRcv, &prm);
        coords[i].tn = offset + i;
    }
}

template <class T>
void sendRight(ExSeis * piol, size_t regionSz, std::vector<T> & rcv)
{
    size_t rank = piol->getRank();
    size_t cnt = regionSz * sizeof(T);

    MPI_Status stat;
    if (rank)
        MPI_Recv(rcv.data(), cnt, MPI_CHAR, rank-1, 1, MPI_COMM_WORLD, &stat);
    if (rank != piol->getNumRank()-1)
        MPI_Send(&rcv[rcv.size()-regionSz], cnt, MPI_CHAR, rank+1, 1, MPI_COMM_WORLD);
}

template <class T>
void sendLeft(ExSeis * piol, size_t regionSz, std::vector<T> & rcv)
{
    size_t rank = piol->getRank();
    size_t cnt = regionSz * sizeof(T);
    MPI_Status stat;
    if (rank)
        MPI_Send(rcv.data(), cnt, MPI_CHAR, rank-1, 0, MPI_COMM_WORLD);
    if (rank != piol->getNumRank()-1)
        MPI_Recv(&rcv[rcv.size()-regionSz], cnt, MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, &stat);
}

bool evencomp(const entry & e1, const entry & e2)
{
    geom_t thresh = 1e-5;
//    auto isEqual = [thresh] (geom_t x, geom_t y) -> bool { return (x > y - thresh) && (x < y + thresh); };

    return (e1.src.x > e2.src.x - thresh && e1.src.x < e2.src.x + thresh && e1.tn < e2.tn) || e1.src.x < e2.src.x;
}

bool paircomp(const std::pair<size_t, size_t> & e1, const std::pair<size_t, size_t> & e2)
{
    return e1.first < e2.first;
}
template <typename T>
void Sort(ExSeis * piol, size_t nt, std::vector<T> & thead, bool (*comp)(const T &, const T &))
{
    size_t regionSz = std::min(nt / (piol->getNumRank() * 4U), std::numeric_limits<int>::max() / sizeof(T));

    size_t numRank = piol->getNumRank();
    size_t rank = piol->getRank();

    size_t edge1 = (rank ? regionSz : 0U);
    size_t edge2 = (rank != numRank-1 ? regionSz : 0U);
    std::vector<T> tSort1(thead.size() + edge2);
    std::vector<T> tSort2(thead.size() + edge2);

    std::sort(thead.begin(), thead.end(), comp);
    std::copy(thead.begin(), thead.end(), tSort1.begin());

    while (numRank > 1) //Infinite loop if there is more than one process, otherwise no loop
    {
        tSort2 = tSort1;
        sendLeft(piol, regionSz, tSort1);

        std::sort(tSort1.begin() + edge1, tSort1.end(), comp);

        sendRight(piol, regionSz, tSort1);

        std::sort(tSort1.begin(), tSort1.end() - edge2, comp);

        int reduced = 0;
        for (int j = 0; j < thead.size() && !reduced; j++)
            reduced += (tSort1[j] != tSort2[j]);
        int greduced = 1;

        int err = MPI_Allreduce(&reduced, &greduced, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

        if (!greduced)
            break;
    }

//    std::vector<size_t> traceNum(thead.size());
    for (size_t i = 0; i < thead.size(); i++)
    {
//        traceNum[i] = tSort1[i].tn;
        thead[i] = tSort1[i];
    }
}

vec<size_t> getSortIndex(size_t sz, size_t * list)
{
    vec<size_t> index(sz);
    std::iota(index.begin(), index.end(), 0);
    std::sort(index.begin(), index.end(), [list] (size_t s1, size_t s2) { return list[s1] < list[s2]; });
    return index;
}

int main(int argc, char ** argv)
{
    ExSeis piol;
    ExSeisPIOL * ppiol = piol;

    std::string opt = "i:o:";  //TODO: uses a GNU extension
    std::string name1 = "";
    std::string name2 = "";
    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'i' :
                name1 = optarg;
            break;
            case 'o' :
                name2 = optarg;
            break;
            default :
                fprintf(stderr, "One of the command line arguments is invalid\n");
            break;
        }
    assert(name1.size() && name2.size());

    size_t rank = piol.getRank();
    size_t numRank = piol.getNumRank();

    File::Direct src(piol, name1, FileMode::Read);

    vec<entry> coords;
    size_t lnt;
    size_t offset;

    //Perform the decomposition and read coordinates of interest.
    {
        auto dec = decompose(src.readNt(), numRank, rank);
        offset = dec.first;
        lnt = dec.second;
        coords.resize(lnt);
        getCoords(src, offset, coords);
    }

    //std::vector<size_t> list =
    Sort(&piol, src.readNt(), coords, evencomp);
    std::vector<std::pair<size_t, size_t>> plist(lnt);
    for (size_t i = 0; i < lnt; i++)
    {
        plist[i].first = coords[i].tn;
        plist[i].second = offset + i;
    }

    Sort(&piol, src.readNt(), plist, paircomp);
    std::vector<size_t> list(lnt);
    for (size_t i = 0; i < lnt; i++)
        list[i] = plist[i].second;


/*    for (size_t r = 0; r < numRank; r++)
    {
        piol.barrier();
        if (r == rank)
            for (size_t i = 0; i < list.size(); i++)
                std::cout << r << " " << list[i] <<  " " << coords[i].src.x << std::endl;
        std::cout << std::flush;
        piol.barrier();
    }*/
    size_t ns = src.readNs();
    File::Direct dst(piol, name2, FileMode::Write);
    dst->writeNt(src.readNt());
    dst->writeInc(src.readInc());
    dst->writeText(src.readText());
    dst->writeNs(ns);

    size_t max = (1024U*1024U*1024U) / (2U*(SEGSz::getDOSz(ns) + sizeof(size_t)));
    size_t biggest = ppiol->comm->max(lnt);
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

    max = std::min(lnt, max);
    vec<trace_t> trc(max * ns);
    vec<trace_t> otrc(max * ns);
    Param prm(max);
    Param oprm(max);


/*    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        vec<size_t> sortlist = getSortIndex(rblock, list.data() + i);
        src->readTrace(rblock, list.data(), trc.data(), &prm);
        dst->writeTrace(offset+i, rblock, trc.data(), &prm);
    }
    for (size_t i = 0; i < extra; i++)
    {
        src->readTrace(0, nullptr, nullptr, const_cast<Param *>(PARAM_NULL));
        dst->writeTrace(0, size_t(0), nullptr, PARAM_NULL);
    }*/

    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        vec<size_t> sortlist = getSortIndex(rblock, list.data() + i);

        src->readTrace(offset + i, rblock, trc.data(), &prm);

        for (size_t j = 0U; j < rblock; j++)
        {
            cpyPrm(j, &prm, sortlist[j], &oprm);

            for (size_t k = 0; k < ns; k++)
                otrc[sortlist[j]*ns + k] = trc[j*ns + k];

            sortlist[j] = list[i+sortlist[j]];
//            std::cout << rank << " " << list[i+j] << " to " << sortlist[j] << std::endl;
        }
        dst->writeTrace(rblock, sortlist.data(), otrc.data(), &oprm);
    }
    for (size_t i = 0; i < extra; i++)
    {
        src->readTrace(0, size_t(0), nullptr, const_cast<Param *>(PARAM_NULL));
        dst->writeTrace(0, nullptr, nullptr, PARAM_NULL);
    }
    return 0;
}
