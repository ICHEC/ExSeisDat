/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \date November 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "cppfileapi.hh"
#include "sglobal.hh"
#include "share/mpi.hh"
#include "4dio.hh"
#include "4dcore.hh"

using namespace PIOL;
using namespace FOURD;
using File::Tr;
using File::Rule;

namespace PIOL {
void cmsg(ExSeisPIOL * piol, std::string msg)
{
    piol->comm->barrier();
    if (!piol->comm->getRank())
        std::cout << msg << std::endl;
}
}

int main(int argc, char ** argv)
{
    ExSeis piol;
    ExSeisPIOL * ppiol = piol;
    geom_t dsrmax = 0.5;            //Default dsdr criteria

/**********************************************************************************************************
 *******************  Reading options from the command line *********************************************** 
 **********************************************************************************************************/
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
                std::cerr<< "One of the command line arguments is invalid\n";
            break;
        }
    assert(name1.size() && name2.size() && name3.size() && name4.size());
/**********************************************************************************************************
 **********************************************************************************************************/

    size_t rank = piol.getRank();
    size_t numRank = piol.getNumRank();

    //Open the two input files
    File::Direct file1(piol, name1, FileMode::Read);
    File::Direct file2(piol, name2, FileMode::Read);

    std::unique_ptr<Coords> coords1;
    std::unique_ptr<Coords> coords2;
    size_t sz[2];

    cmsg(piol, "Parameter-read phase");
    auto startTime = MPI_Wtime();
    //Perform the decomposition and read the coordinates of interest.
    {
        auto dec1 = decompose(file1.readNt(), numRank, rank);
        auto dec2 = decompose(file2.readNt(), numRank, rank);

        sz[0] = dec1.second;

        coords1 = std::make_unique<Coords>(sz[0]);
        assert(coords1.get());
        cmsg(piol, "getCoords1");
        getCoords(ppiol, file1, dec1.first, coords1.get());

        sz[1] = dec2.second;
        cmsg(piol, "getCoords2");
        coords2 = std::make_unique<Coords>(sz[1]);
        getCoords(ppiol, file2, dec2.first, coords2.get());
    }

    vec<size_t> min(sz[0]);
    vec<geom_t> minrs(sz[0]);

    auto szall = ppiol->comm->gather(vec<size_t>{sz[1]});
    std::vector<size_t> offset(szall.size());
    for (size_t i = 1; i < offset.size(); i++)
        offset[i] = offset[i-1] + szall[i];

    recordTime(piol, "parameter", startTime);

#ifdef RANDOM_WRITE
    srand(1337);
    size_t nt = file2.readNt();
    for (size_t i = 0; i < sz[0]; i++)
        min[i] = ((llint(rand()) << 32) & llint(rand())) % nt;
#else
    startTime = MPI_Wtime();
    cmsg(piol, "Round 0 of " + std::to_string(numRank-1));

    auto xsmin = ppiol->comm->gather(vec<geom_t>{coords2->xSrc[0U]});
    auto xsmax = ppiol->comm->gather(vec<geom_t>{coords2->xSrc[coords2->sz-1U]});
    //Perform a local update of min and minrs

#warning It is possible that this is unnecessary
    update<true>(offset[rank], coords1.get(), coords2.get(), min, minrs);
    recordTime(piol, "update 0", startTime);
#warning USE ONE SIDED COMMUNICATION!

    //Perform the updates of min and minrs using data from other processes.
    for (size_t i = 1U; i < numRank; i ++)
    {
        cmsg(piol, "Round " + std::to_string(i) +  " of " + std::to_string(numRank));
        size_t lrank = (rank + numRank - i) % numRank;  //The rank of the left process
        size_t rrank = (rank + i) % numRank;            //The rank of the right process

        //Could the process on the left have data I want?
        size_t IRcv = (xsmax[lrank] + dsrmax > xsmin[rank] && xsmin[lrank] + dsrmax < xsmax[rank]);
        size_t ISnd = (xsmax[rank] + dsrmax > xsmin[rrank] && xsmin[rank] + dsrmax < xsmax[rrank]);

        ppiol->comm->barrier();
        for (size_t j = 0; j < numRank; j++)
        {
            if (j == rank)
                std::cout << rank << " active: " << IRcv << " " << ISnd << std::endl;
            ppiol->comm->barrier();
        }
        startTime = MPI_Wtime();

        //TODO: Check if the other process has data of interest.
        auto proc = std::make_unique<Coords>(szall[lrank]);
        std::vector<MPI_Request> rmsg(4);
        std::vector<MPI_Request> smsg(4);

        if (IRcv)
        {
            //Receive data from the process on the left
            MPI_Irecv(proc->xSrc, szall[lrank], MPIType<geom_t>(), lrank, lrank, MPI_COMM_WORLD, &rmsg[0]);
            MPI_Irecv(proc->ySrc, szall[lrank], MPIType<geom_t>(), lrank, lrank, MPI_COMM_WORLD, &rmsg[1]);
            MPI_Irecv(proc->xRcv, szall[lrank], MPIType<geom_t>(), lrank, lrank, MPI_COMM_WORLD, &rmsg[2]);
            MPI_Irecv(proc->yRcv, szall[lrank], MPIType<geom_t>(), lrank, lrank, MPI_COMM_WORLD, &rmsg[3]);
            for (size_t j = 0; j < rmsg.size(); j++)
                assert(rmsg[0] != MPI_REQUEST_NULL);
        }
        if (ISnd)  //This could be done one sided in a tidier way?
        {
            //Send data to the process on the right
            MPI_Isend(coords2->xSrc, szall[rank], MPIType<geom_t>(), rrank, rank, MPI_COMM_WORLD, &smsg[0]);
            MPI_Isend(coords2->ySrc, szall[rank], MPIType<geom_t>(), rrank, rank, MPI_COMM_WORLD, &smsg[1]);
            MPI_Isend(coords2->xRcv, szall[rank], MPIType<geom_t>(), rrank, rank, MPI_COMM_WORLD, &smsg[2]);
            MPI_Isend(coords2->yRcv, szall[rank], MPIType<geom_t>(), rrank, rank, MPI_COMM_WORLD, &smsg[3]);
            for (size_t j = 0; j < smsg.size(); j++)
                assert(smsg[0] != MPI_REQUEST_NULL);
        }

        MPI_Status stat;
        if (IRcv)
            for (size_t j = 0; j < rmsg.size(); j++)
                assert(MPI_Wait(&rmsg[j], &stat) == MPI_SUCCESS);         //TODO: Replace with standard approach to error handling

        recordTime(piol, "snd/rcv " + std::to_string(i), startTime);
        startTime = MPI_Wtime();

        if (IRcv)
            update<false>(offset[lrank], coords1.get(), proc.get(), min, minrs);

        recordTime(piol, "compute " + std::to_string(i), startTime);

        if (ISnd)
            for (size_t j = 0; j < smsg.size(); j++)
                assert(MPI_Wait(&smsg[j], &stat) == MPI_SUCCESS);         //TODO: Replace with standard approach to error handling

    }
#endif
    //free up some memory
    coords1.release();
    coords2.release();

    size_t cnt = 0U;
    vec<size_t> list1(sz[0]);
    vec<size_t> list2(sz[0]);
    cmsg(piol, "Final list pass");

//Weed out traces that have a match thatt is too far away
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

    //Enable as many of the parameters as possible
    auto rule = std::make_shared<Rule>(true, true, true);
    rule->addFloat(Meta::dsdr, Tr::SrcMeas, Tr::SrcMeasExp);

    cmsg(piol, "Output phase");

    auto outTime = MPI_Wtime();

    startTime = MPI_Wtime();
    //Open and write out file1 --> file3
    File::Direct file3(piol, name3, FileMode::Write);

    select(piol, rule, file3, file1, list1, minrs);

    recordTime(piol, "First output", startTime);
    startTime = MPI_Wtime();
    //Open and write out file2 --> file4
    //This case is more complicated because the list is unordered and there  can be duplicate entries
    //in the list.
    File::Direct file4(piol, name4, FileMode::Write);
    selectDupe(piol, rule, file4, file2, list2, minrs);

    recordTime(piol, "Second output", startTime);
    return 0;
}
