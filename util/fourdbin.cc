#include <assert.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "cppfileapi.hh"
#include "sglobal.hh"
#include "share/smpi.hh"
using namespace PIOL;
using File::Meta;
using File::Tr;
using File::Rule;

template <class T>
using cvec = const std::vector<T>;

template <class T>
using vec = std::vector<T>;

inline geom_t dsr(const geom_t * prm1, const geom_t * prm2)
{
    return std::abs(prm1[0] - prm2[0]) +
           std::abs(prm1[1] - prm2[1]) +
           std::abs(prm1[2] - prm2[2]) +
           std::abs(prm1[3] - prm2[3]);
}

void getCoords(File::Interface * file, size_t offset, vec<geom_t> & coords)
{
    size_t sz = coords.size()/4U;
    vec<Meta> m = {Meta::xSrc, Meta::ySrc, Meta::xRcv, Meta::yRcv};
    auto rule = std::make_shared<Rule>(true, m);

    File::Param prm(rule, sz);
    file->readParam(offset, sz, &prm);
    for (size_t i = 0; i < sz; i++)
    {
        coords[4U*i+0] = File::getPrm<geom_t>(i, m[0], &prm);
        coords[4U*i+1] = File::getPrm<geom_t>(i, m[1], &prm);
        coords[4U*i+2] = File::getPrm<geom_t>(i, m[2], &prm);
        coords[4U*i+3] = File::getPrm<geom_t>(i, m[3], &prm);
    }
}

template <bool Init>
void update(cvec<size_t> & szall, cvec<geom_t> & local,
            size_t orank, cvec<geom_t> & other, vec<size_t> & min, vec<geom_t> & minrs)
{
    size_t sz = local.size() / 4U;

    size_t offset = 0;
    for (size_t i = 0; i < orank; i++)
        offset += szall[i];

    if (Init)
        for (size_t i = 0; i < sz; i++)
        {
            geom_t dval = dsr(&local[4U*i], &other[0U]);
            minrs[i] = dval;
            min[i] = offset;
        }

    for (size_t j = (Init ? 1U : 0U); j < szall[orank]; j++)
        for (size_t i = 0; i < sz; i++)
        {
            geom_t dval = dsr(&local[4U*i], &other[4U*j]);
//TODO BENCHMARK CONDITIONAL vs TERNARY?
            if (dval < minrs[i])
            {
                minrs[i] = dval;
                min[i] = offset + j;
            }
        }
}

//This trick is discussed here:
//http://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
vec<size_t> getSortIndex(size_t sz, size_t * list)
{
    vec<size_t> index(sz);
    std::iota(index.begin(), index.end(), 0);
    std::sort(index.begin(), index.end(), [list] (size_t s1, size_t s2) { return list[s1] < list[s2]; });
    return index;
}

//TODO: Have a mechanism to change from one Param representation to another?

void selectDupe(ExSeisPIOL * piol, File::Direct & dst, File::Direct & src, vec<size_t> & list, vec<geom_t> & minrs)
{
    size_t ns = src.readNs();
    size_t lnt = list.size();
    size_t offset = 0;
    size_t biggest = 0;
    size_t sz = 0;
    {
        auto nts = piol->comm->gather(vec<size_t>{lnt});
        for (size_t i = 0; i < nts.size(); i++)
        {
            if (i == piol->comm->getRank())
                offset = sz;
            sz += nts[i];
            biggest = std::max(biggest, nts[i]);
        }
    }
    vec<Meta> m = {Meta::xSrc, Meta::ySrc, Meta::xRcv, Meta::yRcv};
    auto rule = std::make_shared<Rule>(true, m);

    size_t memused = lnt * (sizeof(size_t) + sizeof(geom_t));
    size_t memlim = 2U*1024U*1024U*1024U;
    assert(memlim > memused);
    size_t max = (memlim - memused) / (4U*SEGSz::getDOSz(ns) + 4U*rule->extent());
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

    dst.writeText("ExSeisDat 4d-bin file.\n");
    dst.writeNt(sz);
    dst.writeInc(src.readInc());
    dst.writeNs(ns);

    File::Param prm(rule, std::min(lnt, max));
    vec<trace_t> trc(ns * std::min(lnt, max));

    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);

        auto idx = getSortIndex(rblock, &list[i]);
        std::vector<size_t> nodups;
        for (size_t j = 0; j < rblock; j++)
            if (!j || list[idx[j-1]] != list[idx[j]])
                nodups.push_back(list[idx[j]]);

        File::Param sprm(rule, nodups.size());
        vec<trace_t> strc(ns * nodups.size());

        if (!piol->comm->getRank())
            std::cout << "read " << nodups.size() << std::endl;

        src.readTrace(nodups.size(), nodups.data(), strc.data(), &sprm);

        if (!piol->comm->getRank())
            std::cout << "process\n";

        size_t n = 0;
        for (size_t j = 0; j < rblock; j++)
        {
            if (!j || list[idx[j-1]] != list[idx[j]])
                n = j;
            cpyPrm(n, &sprm, idx[n], &prm);

            setPrm(j, Meta::dsdr, minrs[j], &prm);
            for (size_t k = 0; k < ns; k++)
                trc[j*ns + k] = strc[idx[n]*ns + k];
        }
        if (!piol->comm->getRank())
            std::cout << "write\n";

        dst.writeTrace(offset+i, rblock, trc.data(), &prm);
    }

    for (size_t i = 0; i < extra; i++)
    {
        src.readTrace(0, nullptr, nullptr, const_cast<File::Param *>(File::PARAM_NULL));
        dst.writeTrace(0, size_t(0), nullptr, File::PARAM_NULL);
    }
}


void select(ExSeisPIOL * piol, File::Direct & dst, File::Direct & src, vec<size_t> & list, vec<geom_t> & minrs)
{
    const size_t ns = src.readNs();
    const size_t lnt = list.size();
    dst.writeText("ExSeisDat 4d-bin file.\n");
    size_t offset = 0;
    size_t biggest = 0;
    size_t sz = 0;
    {
        auto offsets = piol->comm->gather(vec<size_t>{lnt});
        for (size_t i = 0; i < offsets.size(); i++)
        {
            if (i == piol->comm->getRank())
                offset = sz;
            sz += offsets[i];
            biggest = std::max(biggest, offsets[i]);
        }
    }
    vec<Meta> m = {Meta::xSrc, Meta::ySrc, Meta::xRcv, Meta::yRcv};
    auto rule = std::make_shared<Rule>(true, m);

    size_t memused = lnt * (sizeof(size_t) + sizeof(geom_t));
    size_t memlim = 2U*1024U*1024U*1024U;
    assert(memlim > memused);

    size_t max = (memlim - memused) / (4U*SEGSz::getDOSz(ns) + 4U*rule->extent());
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

    dst.writeNt(sz);
    dst.writeInc(src.readInc());
    dst.writeNs(src.readNs());

/*    for (size_t i = 0; i < piol->comm->getNumRank(); i++)
    {
        piol->comm->barrier();
        if (i == piol->comm->getRank())
            std::cout << i << " " << biggest << " " << lnt << "  " << max << " " << extra << std::endl;
        piol->comm->barrier();
    }*/

    File::Param prm(rule, std::min(lnt, max));
    vec<trace_t> trc(src.readNs() * std::min(lnt, max));

    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);

        if (!piol->comm->getRank())
            std::cout << "R Block = " << rblock << std::endl;

        src.readTrace(rblock, &list[i], trc.data(), &prm);
        for (size_t j = 0; j < rblock; j++)
            setPrm(j, Meta::dsdr, minrs[j], &prm);
        dst.writeTrace(offset+i, rblock, trc.data(), &prm);
    }

    for (size_t i = 0; i < extra; i++)
    {
        src.readTrace(0, nullptr, nullptr, const_cast<File::Param *>(File::PARAM_NULL));
        dst.writeTrace(0, size_t(0), nullptr, File::PARAM_NULL);
    }
}


int main(int argc, char ** argv)
{
    ExSeis piol;
    ExSeisPIOL * ppiol = piol;
    geom_t dsrmax = 0.5;

    std::string opt = "a:b:c:d:t:";  //TODO: uses a GNU extension
    std::string name1 = "";
    std::string name2 = "";
    std::string name3 = "";
    std::string name4 = "";
    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'a' :
                name1 = optarg;
            break;
            case 'b' :
                name2 = optarg;
            break;
            case 'c' :
                name3 = optarg;
            break;
            case 'd' :
                name4 = optarg;
            break;
            case 't' :
                dsrmax = std::stod(optarg);
            break;
            default :
                fprintf(stderr, "One of the command line arguments is invalid\n");
            break;
        }
    assert(name1.size() && name2.size() && name3.size() && name4.size());

    size_t rank = piol.getRank();
    size_t numRank = piol.getNumRank();

    std::vector<Meta> m = {Meta::xSrc, Meta::ySrc, Meta::xRcv, Meta::yRcv};
    auto rule = std::make_shared<File::Rule>(true, m);
    rule->addFloat(Meta::dsdr, Tr::SrcMeas, Tr::SrcMeasExp);
    File::Direct file1(piol, name1, FileMode::Read, rule);
    File::Direct file2(piol, name2, FileMode::Read, rule);

    vec<geom_t> coords1;
    vec<geom_t> coords2;
    size_t sz[2];

    //Perform the decomposition and read coordinates of interest.
    {
        auto dec1 = decompose(file1.readNt(), numRank, rank);
        auto dec2 = decompose(file2.readNt(), numRank, rank);

        sz[0] = dec1.second;
        coords1.resize(4U*sz[0]);
        getCoords(file1, dec1.first, coords1);

        sz[1] = dec2.second;
        coords2.resize(4U*sz[1]);
        getCoords(file2, dec2.first, coords2);
    }

    vec<size_t> min(sz[0]);
    vec<geom_t> minrs(sz[0]);
#define RANDOM_WRITE
#ifdef RANDOM_WRITE
    srand(1337);
    for (size_t nt = file2.readNt(), i = 0; i < sz[0]; i++)
        min[i] = ((llint(rand()) << 8) & llint(rand())) % nt;
#else
    auto szall = ppiol->comm->gather(vec<size_t>{sz[1]});

    //Perform a local update of min and minrs
    if (!rank)
        std::cout << "Round " << 1 << " of " << numRank << std::endl;
    update<true>(szall, coords1, rank, coords2, min, minrs);

    //Perform the updates of min and minrs using data from other processes.
    for (size_t i = 1U; i < numRank; i ++)
    {
        if (!rank)
            std::cout << "Round " << i+1 << " of " << numRank << std::endl;
        size_t lrank = (rank + numRank - i) % numRank;  //The rank of the left process
        size_t rrank = (rank + i) % numRank;            //The rank of the right process

        //TODO: Check if the other process has data of interest.

        vec<geom_t> proc(4U*szall[lrank]);
        MPI_Request msg[2];
        //Receive data from the process on the left
        MPI_Irecv(proc.data(), 4U*szall[lrank], MPIType<geom_t>(), lrank, lrank, MPI_COMM_WORLD, &msg[0]);

        //Send data to the process on the right
        MPI_Isend(coords2.data(), 4U*szall[rank], MPIType<geom_t>(), rrank, rank, MPI_COMM_WORLD, &msg[1]);

        MPI_Status stat;
        assert(msg[0] != MPI_REQUEST_NULL);
        assert(msg[1] != MPI_REQUEST_NULL);
        assert(MPI_Wait(&msg[0], &stat) == MPI_SUCCESS);         //TODO: Replace with standard approach to error handling

        update<false>(szall, coords1, lrank, proc, min, minrs);
        assert(MPI_Wait(&msg[1], &stat) == MPI_SUCCESS);         //TODO: Replace with standard approach to error handling
    }
#endif

    coords1.resize(0);
    coords2.resize(0);
    coords1.shrink_to_fit();
    coords2.shrink_to_fit();

    size_t cnt = 0U;
    vec<size_t> list1(sz[0]);
    vec<size_t> list2(sz[0]);
    if (!rank)
        std::cout << "Final list pass\n";
    for (size_t i = 0U; i < sz[0]; i++)
    {
        if (minrs[i] <= dsrmax)
        {
            list2[cnt] = min[i];
            list1[cnt++] = i;
        }
    }
    list1.resize(cnt);
    list2.resize(cnt);

    if (!rank)
        std::cout << "Select s3\n";

    File::Direct file3(piol, name3, FileMode::Write, rule);
    select(piol, file3, file1, list1, minrs);

    if (!rank)
        std::cout << "Select s4\n";

    File::Direct file4(piol, name4, FileMode::Write, rule);
    selectDupe(piol, file4, file2, list2, minrs);

    if (!rank)
        std::cout << "Done\n";

    return 0;
}
