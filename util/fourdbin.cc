#include <assert.h>
#include <cmath>
#include <iostream>
#include "cppfileapi.hh"
#include "sglobal.hh"
#include "share/smpi.hh"
using namespace PIOL;
using File::Meta;
using File::Tr;

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
    File::Param prm(file->getRule(), sz);
    file->readParam(offset, sz, &prm);
    for (size_t i = 0; i < sz; i++)
    {
        coords[4U*i+0] = getPrm(file->getRule(), i, Meta::xSrc, &prm);
        coords[4U*i+1] = getPrm(file->getRule(), i, Meta::ySrc, &prm);
        coords[4U*i+2] = getPrm(file->getRule(), i, Meta::xRcv, &prm);
        coords[4U*i+3] = getPrm(file->getRule(), i, Meta::yRcv, &prm);
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
            if (dval < minrs[i])
            {
                minrs[i] = dval;
                min[i] = offset + j;
            }
        }
}

//Non-Contiguous read, contiguous write
void select(ExSeisPIOL * piol, File::Direct & dst, File::Direct & src, vec<size_t> & list, vec<geom_t> & minrs)
{
    const size_t sz = list.size();
    dst.writeText("ExSeisDat 4d-bin file.\n");
    dst.writeNt(sz);
    dst.writeInc(src.readInc());
    dst.writeNs(src.readNs());

    File::Param prm(src.getRule(), sz);
    vec<trace_t> trc(sz * src.readNs());

    src.readTrace(sz, list.data(), trc.data(), &prm);

//TODO: Have a mechanism to change from one representation to another?
    for (size_t i = 0; i < sz; i++)
        setPrm(dst.getRule(), i, Meta::dsdr, minrs[i], &prm);

//TODO: Replace with MPI call
    auto offsets = piol->comm->gather(vec<size_t>{list.size()});
    size_t offset = 0;
    for (size_t i = 0; i < piol->comm->getRank(); i++)
        offset += offsets[i];

    dst.writeTrace(offset, sz, trc.data(), &prm);
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

    size_t cnt = 0U;
    vec<size_t> list1(sz[0]);
    vec<size_t> list2(sz[0]);
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

    File::Direct file3(piol, name3, FileMode::Write, rule);
    select(piol, file3, file1, list1, minrs);

    File::Direct file4(piol, name4, FileMode::Write, rule);
    select(piol, file4, file2, list2, minrs);
    return 0;
}
