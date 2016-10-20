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
        for (size_t j = Init ? 1U : 0U; j < szall[orank]; j++)
        {
            geom_t sr = dsr(&local[4U*i], &other[4U*j]);
            if (sr < minrs[i])
            {
                minrs[i] = sr;
                min[i] = offset + j;
            }
        }
}

int main(void)
{
    std::string name1 = "s1.segy";
    std::string name2 = "s2.segy";
    std::string name3 = "s3.segy";
    std::string name4 = "s4.segy";

    ExSeis piol;

    size_t rank = piol.getRank();
    size_t numRank = piol.getNumRank();
    if (!rank)
        std::cout << "Piol initialised\n";

    ExSeisPIOL * ppiol = piol;

    File::Direct file1(piol, name1, FileMode::Read);
    File::Direct file2(piol, name2, FileMode::Read);

    if (!rank)
        std::cout << "Input files opened\n";

    vec<geom_t> coords1;
    vec<geom_t> coords2;
    size_t offset[2];
    size_t sz[2];
    {
        auto dec1 = decompose(file1.readNt(), numRank, rank);
        auto dec2 = decompose(file2.readNt(), numRank, rank);
        sz[0] = dec1.second;
        sz[1] = dec2.second;
        offset[0] = dec1.first;
        offset[1] = dec2.first;

        if (!rank)
            std::cout << "Decomposition done " << sz[0] << " " << file1.readNt() << " " << sz[1] << " " << file2->readNt() << std::endl;

        coords1.resize(4U*sz[0]);
        getCoords(file1, offset[0], coords1);

        coords2.resize(4U*sz[1]);
        getCoords(file2, offset[1], coords2);
    }
    vec<size_t> min(sz[0]);
    vec<geom_t> minrs(sz[0]);

    if (!rank)
        std::cout << "Traces Read\n";

    auto szall = ppiol->comm->gather(vec<size_t>{sz[1]});
    update<true>(szall, coords1, rank, coords2, min, minrs);
    for (size_t i = 1; i < numRank; i ++)
    {
        size_t lrank = (rank + numRank - i) % numRank;  //The rank of the left process
        size_t rrank = (rank + i) % numRank;            //The rank of the right process

        //TODO: Check if it is possible that the other process has data of interest.
        vec<geom_t> proc(4U*szall[lrank]);
        MPI_Request msg[2];
        MPI_Irecv(proc.data(), 4U*szall[lrank], MPIType<geom_t>(), lrank, lrank, MPI_COMM_WORLD, &msg[0]);
        MPI_Isend(coords2.data(), 4U*szall[rank], MPIType<geom_t>(), rrank, rank, MPI_COMM_WORLD, &msg[1]);

        MPI_Status stat;
        assert(msg[0] != MPI_REQUEST_NULL);
        assert(msg[1] != MPI_REQUEST_NULL);
        assert(MPI_Wait(&msg[0], &stat) == MPI_SUCCESS);         //TODO: Replace with standard approach to error handling
        assert(MPI_Wait(&msg[1], &stat) == MPI_SUCCESS);         //TODO: Replace with standard approach to error handling

        update<false>(szall, coords1, lrank, proc, min, minrs);
    }

    for (size_t r = 0; r < numRank; r++)
    {
        if (r == rank)
            for (size_t i = 0; i < sz[0]; i++)
                std::cout << " " <<  min[i] << " " << minrs[i] << std::endl;
        piol.barrier();
    }
    return 0;
}

