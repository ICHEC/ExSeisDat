#include <cmath>
#include <assert.h>
#include "cppfileapi.hh"
#include "sglobal.hh"
#include "share/smpi.hh"
using namespace PIOL;

geom_t dsr(const geom_t * prm1, const geom_t * prm2)
{
    return std::abs(prm1[0] - prm2[0]) +
           std::abs(prm1[1] - prm2[1]) +
           std::abs(prm1[2] - prm2[2]) +
           std::abs(prm1[3] - prm2[3]);
}

void getCoords(File::Interface * file, size_t offset, std::vector<geom_t> & coords)
{
    size_t sz = coords.size()/4U;
    std::vector<File::TraceParam> prm(sz);
    file->readTraceParam(offset, sz, prm.data());
    for (size_t i = 0; i < sz; i++)
    {
        coords[4U*i+0] = prm[i].src.x;
        coords[4U*i+1] = prm[i].src.y;
        coords[4U*i+2] = prm[i].rcv.x;
        coords[4U*i+3] = prm[i].rcv.y;
    }
}

template <class T>
using cvec = const std::vector<T>;

template <class T>
using vec = std::vector<T>;

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

    for (size_t i = 0; i < sz; i++)
        for (size_t j = (Init ? 1U : 0U); j < szall[orank]; j++)
        {
            geom_t dval = dsr(&local[4U*i], &other[4U*j]);
            if ((dval < minrs[i]) || (dval == minrs[i] && min[i] > offset + j))
            {
                minrs[i] = dval;
                min[i] = offset + j;
            }
        }
}

//Non-Contiguous read, contiguous write
void select(ExSeisPIOL * piol, File::Interface * dst, File::Interface * src, vec<size_t> & list)
{
    const size_t sz = list.size();
    dst->writeText("ExSeisDat 4d-bin file.\n");
    dst->writeNt(sz);
    dst->writeInc(src->readInc());
    dst->writeNs(src->readNs());

    vec<File::TraceParam> prm(sz);
    vec<trace_t> trc(sz * src->readNs());

    src->readTrace(sz, list.data(), trc.data(), prm.data());

    auto offsets = piol->comm->gather(vec<size_t>{list.size()});
    size_t offset = 0;
    for (size_t i = 0; i < piol->comm->getRank(); i++)
        offset += offsets[i];

    dst->writeTrace(offset, sz, trc.data(), prm.data());
}

int main(void)
{
    ExSeis piol;
    ExSeisPIOL * ppiol = piol;
    std::string name1 = "s1.segy";
    std::string name2 = "s2.segy";
    std::string name3 = "s3.segy";
    std::string name4 = "s4.segy";

    size_t rank = piol.getRank();
    size_t numRank = piol.getNumRank();

    File::Direct file1(piol, name1, FileMode::Read);
    File::Direct file2(piol, name2, FileMode::Read);

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
    update<true>(szall, coords1, rank, coords2, min, minrs);

    //Perform the updates of min and minrs using data from other processes.
    for (size_t i = 1U; i < numRank; i ++)
    {
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

    const geom_t dsrmax = 0.5;
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

    File::Direct file3(piol, name3, FileMode::Write);
    File::Direct file4(piol, name4, FileMode::Write);

    select(piol, file3, file1, list1);
    select(piol, file4, file2, list2);
    return 0;
}

