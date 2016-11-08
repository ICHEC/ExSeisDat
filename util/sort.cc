#include <assert.h>
#include <cmath>
#include <iostream>
#include <algorithm>
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

void sendRight(ExSeis * piol, size_t regionSz, std::vector<entry> & rcv)
{
    size_t rank = piol->getRank();
    size_t cnt = regionSz * sizeof(entry);

    MPI_Status stat;
    if (rank)
        MPI_Recv(rcv.data(), cnt, MPI_CHAR, rank-1, 1, MPI_COMM_WORLD, &stat);
    if (rank != piol->getNumRank()-1)
        MPI_Send(&rcv[rcv.size()-regionSz], cnt, MPI_CHAR, rank+1, 1, MPI_COMM_WORLD);
}

void sendLeft(ExSeis * piol, size_t regionSz, std::vector<entry> & rcv)
{
    size_t rank = piol->getRank();
    size_t cnt = regionSz * sizeof(entry);
    MPI_Status stat;
    if (rank)
        MPI_Send(rcv.data(), cnt, MPI_CHAR, rank-1, 0, MPI_COMM_WORLD);
    if (rank != piol->getNumRank()-1)
        MPI_Recv(&rcv[rcv.size()-regionSz], cnt, MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, &stat);
}

bool comp(const entry & e1, const entry & e2)
{
    geom_t thresh = 1e-5;
    auto isEqual = [thresh] (geom_t x, geom_t y) -> bool { return (x > y - thresh) && (x < y + thresh); };

    return (e1.src.x > e2.src.x - thresh && e1.src.x < e2.src.x + thresh && e1.tn < e2.tn) || e1.src.x < e2.src.x;
}

std::vector<size_t> Sort(ExSeis * piol, size_t nt, std::vector<entry> & thead)
{
    size_t regionSz = std::min(nt / (piol->getNumRank() * 4U), std::numeric_limits<int>::max() / sizeof(entry));

    size_t numRank = piol->getNumRank();
    size_t rank = piol->getRank();

    size_t edge1 = (rank ? regionSz : 0U);
    size_t edge2 = (rank != numRank-1 ? regionSz : 0U);
    std::vector<entry> tSort1(thead.size() + edge2);
    std::vector<entry> tSort2(thead.size() + edge2);

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
            reduced += (tSort1[j].tn != tSort2[j].tn);
        int greduced = 1;

        int err = MPI_Allreduce(&reduced, &greduced, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

        if (!greduced)
            break;
    }

    std::vector<size_t> traceNum(thead.size());
    for (size_t i = 0; i < thead.size(); i++)
    {
        traceNum[i] = tSort1[i].tn;
        thead[i] = tSort1[i];
    }
    return traceNum;
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
    size_t sz;
    size_t offset;

    //Perform the decomposition and read coordinates of interest.
    {
        auto dec = decompose(src.readNt(), numRank, rank);
        offset = dec.first;
        sz = dec.second;
        coords.resize(sz);
        getCoords(src, dec.first, coords);
    }

    std::vector<size_t> list = Sort(&piol, src.readNt(), coords);

    for (size_t r = 0; r < numRank; r++)
    {
        piol.barrier();
        if (r == rank)
            for (size_t i = 0; i < list.size(); i++)
                std::cout << r << " " << list[i] <<  " " << coords[i].src.x << std::endl;
        std::cout << std::flush;
        piol.barrier();
    }
    return 0;
    File::Param prm(sz);
    vec<trace_t> trc(sz * src->readNs());
    src->readTrace(offset, sz, trc.data(), &prm);

    File::Direct dst(piol, name2, FileMode::Write);
    dst->writeNt(src->readNt());
    dst->writeInc(src->readInc());
    dst->writeText(src->readText());
    dst->writeNs(src->readNs());

//TODO: Not ok for big writes
    dst->writeTrace(sz, list.data(), trc.data(), &prm);

    return 0;
}
