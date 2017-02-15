/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \date November 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#warning temp
#include <stdio.h>

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


/*! Create windows for one-sided communication of coordinates
 *  \param[in] coords The coordinate structure of arrays to open to RDMA.
 */
vec<MPI_Win> createCoordsWindow(Coords * coords)
{
    std::vector<MPI_Win> win(5);
    //Look at MPI_Info
    int err;
    err = MPI_Win_create(coords->xSrc, coords->sz, sizeof(geom_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win[0]);
    assert(err == MPI_SUCCESS);
    err = MPI_Win_create(coords->ySrc, coords->sz, sizeof(geom_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win[1]);
    assert(err == MPI_SUCCESS);
    err = MPI_Win_create(coords->xRcv, coords->sz, sizeof(geom_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win[2]);
    assert(err == MPI_SUCCESS);
    err = MPI_Win_create(coords->yRcv, coords->sz, sizeof(geom_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win[3]);
    assert(err == MPI_SUCCESS);
    err = MPI_Win_create(coords->tn, coords->sz, sizeof(size_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win[4]);
    assert(err == MPI_SUCCESS);
    return win;
}

/*! One-sided retrieval from data in window on processor lrank. Processor lrank is passive.
 *  \param[in] lrank The rank
 *  \param[in] sz The number of coordinates
 *  \param[in] win The vector of windows to access with.
 *  \return Return the associated Coords structure
 */
std::unique_ptr<Coords> getCoordsWin(size_t lrank, size_t sz, vec<MPI_Win> & win)
{
    auto coords = std::make_unique<Coords>(sz);
    for (size_t i = 0; i < 5; i++)
        MPI_Win_lock(MPI_LOCK_SHARED, lrank, 0, win[i]);
    int err;
    err = MPI_Get(coords->xSrc, coords->sz, MPIType<geom_t>(), lrank, 0, sz, MPIType<geom_t>(), win[0]);
    assert(err == MPI_SUCCESS);
    err = MPI_Get(coords->ySrc, coords->sz, MPIType<geom_t>(), lrank, 0, sz, MPIType<geom_t>(), win[1]);
    assert(err == MPI_SUCCESS);
    err = MPI_Get(coords->xRcv, coords->sz, MPIType<geom_t>(), lrank, 0, sz, MPIType<geom_t>(), win[2]);
    assert(err == MPI_SUCCESS);
    err = MPI_Get(coords->yRcv, coords->sz, MPIType<geom_t>(), lrank, 0, sz, MPIType<geom_t>(), win[3]);
    assert(err == MPI_SUCCESS);
    err = MPI_Get(coords->tn, coords->sz, MPIType<size_t>(), lrank, 0, sz, MPIType<size_t>(), win[4]);
    assert(err == MPI_SUCCESS);
    for (size_t i = 0; i < 5; i++)
        MPI_Win_unlock(lrank, win[i]);
    return std::move(coords);
}

void printxSrcMinMax(ExSeisPIOL * piol, vec<geom_t> & xsmin, vec<geom_t> & xsmax)
{
    piol->comm->barrier();
    assert(xsmin.size() == xsmax.size());
    if (!piol->comm->getRank())
        for (size_t i = 0; i < xsmin.size(); i++)
            std::cout << "minmax " << i << " " << xsmin[i] << " " << xsmax[i] << std::endl;
    piol->comm->barrier();
}

int main(int argc, char ** argv)
{
    ExSeis piol;
    ExSeisPIOL * ppiol = piol;
    geom_t dsrmax = 0.5;            //Default dsdr criteria
    bool verbose = false;
/**********************************************************************************************************
 *******************  Reading options from the command line *********************************************** 
 **********************************************************************************************************/
    std::string opt = "a:b:c:d:t:v";  //TODO: uses a GNU extension
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
            case 'v' :
                verbose = true;
                cmsg(piol, "Verbose mode enabled");
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

/*****************************************************************************/
/***************/ cmsg(piol, "Parameter-read phase"); /***********************/
/*****************************************************************************/

    //Perform the decomposition and read the coordinates of interest.
    auto time = MPI_Wtime();

    {
        auto dec1 = decompose(file1.readNt(), numRank, rank);
        auto dec2 = decompose(file2.readNt(), numRank, rank);

        sz[0] = dec1.second;
        sz[1] = dec2.second;

#ifndef HACK
        coords1 = std::make_unique<Coords>(sz[0]);
        assert(coords1.get());
        getCoords(ppiol, file1, dec1.first, coords1.get());

        cmsg(piol, "Read sets of coordinates from file " + name1 + " in " + std::to_string(MPI_Wtime()- time) + " seconds");
        time = MPI_Wtime();

        coords2 = std::make_unique<Coords>(sz[1]);
        getCoords(ppiol, file2, dec2.first, coords2.get());

        cmsg(piol, "Read sets of coordinates from file " + name2 + " in " + std::to_string(MPI_Wtime()- time) + " seconds");
#endif
    }
#ifndef HACK
/*****************************************************************************/
/********************/ cmsg(piol, "Compute phase"); /*************************/
/*****************************************************************************/
    vec<size_t> min(sz[0]);
    vec<geom_t> minrs(sz[0]);

    auto szall = ppiol->comm->gather(vec<size_t>{sz[1]});
    std::vector<size_t> offset(szall.size());
    for (size_t i = 1; i < offset.size(); i++)
        offset[i] = offset[i-1] + szall[i];

    //The File2 min/max from every process
    auto xsmin = ppiol->comm->gather(vec<geom_t>{coords2->xSrc[0U]});
    auto xsmax = ppiol->comm->gather(vec<geom_t>{coords2->xSrc[coords2->sz-1U]});

    //The File1 local min and local maximum for the particular process
    auto xslmin = coords1->xSrc[0U];
    auto xslmax = coords1->xSrc[coords1->sz-1U];

    //Perform a local initialisation update of min and minrs
    initUpdate(offset[rank], coords1.get(), coords2.get(), min, minrs);
    //recordTime(piol, "update 0", startTime);

    //This for loop determines the processes the local process will need to be communicating with.
    std::vector<size_t> active;
    for (size_t i = 0U; i < numRank; i++)
        if ((xsmin[i] - dsrmax <= xslmax) && (xsmax[i] + dsrmax >= xslmin))
            active.push_back(i);

    if (verbose)
    {
        std::string name = "tmp/temp" + std::to_string(rank);
        FILE * fOut = fopen(name.c_str(), "w+");
        fprintf(fOut, "1-xsmin/max %f %f\n", xslmin, xslmax);
        fprintf(fOut, "2-xsmin/max %f %f\n", xsmin[rank], xsmax[rank]);
        for (size_t i = 0; i < active.size(); i++)
            fprintf(fOut, "%zu\n", active[i]);
        fclose(fOut);

        auto lxsmin = ppiol->comm->gather(vec<geom_t>{xslmin});
        auto lxsmax = ppiol->comm->gather(vec<geom_t>{xslmax});

        printxSrcMinMax(piol, lxsmin, lxsmax);
        if (!rank)
            std::cout << "file2 min/max\n";
        printxSrcMinMax(piol, xsmin, xsmax);
    }

////////////////////// COMPUTE //////////////////////////////////////////////

    time = MPI_Wtime();
    auto win = createCoordsWindow(coords2.get());

    //Perform the updates of min and minrs using data from other processes.
    //This is the main loop.
    for (size_t i = 0; i < active.size(); i ++)
    {
        auto ltime = MPI_Wtime();
        size_t lrank = active[i];
        auto proc = getCoordsWin(lrank, szall[lrank], win);
        update(offset[lrank], coords1.get(), proc.get(), min, minrs);
    }

    for (size_t i = 0; i < win.size(); i++)
    {
        int err = MPI_Win_free(&win[i]);
        assert(err == MPI_SUCCESS);
    }

    cmsg(piol, "Compute phase completed in " + std::to_string(MPI_Wtime() - time) + " seconds");

///////////////////////////////////////////////////////////////////////////////

    cmsg(piol, "Final list pass");
//Weed out traces that have a match that is too far away

    vec<size_t> list1(sz[0]);
    vec<size_t> list2(sz[0]);
    vec<geom_t> lminrs(sz[0]);
    size_t cnt = 0U;
    for (size_t i = 0U; i < sz[0]; i++)
        if (minrs[i] <= dsrmax)
        {
            list2[cnt] = min[i];
            lminrs[cnt] = minrs[i];
            list1[cnt++] = coords1->tn[i];
        }

    list1.resize(cnt);
    list2.resize(cnt);
    lminrs.resize(cnt);

{
    std::string name = "tmp/vtn" + std::to_string(rank);
    FILE * fOut = fopen(name.c_str(), "w+");
    fwrite(list1.data(), list1.size(), sizeof(size_t), fOut);
    fwrite(list2.data(), list2.size(), sizeof(size_t), fOut);
    fwrite(lminrs.data(), lminrs.size(), sizeof(geom_t), fOut);
    fclose(fOut);

    name += ".txt";
    fOut = fopen(name.c_str(), "w+");
    fprintf(fOut, "N#\t\tlist1\t\tlist2\t\tminrs\n");
    for (size_t i = 0; i < list1.size(); i++)
        fprintf(fOut, "%zu\t\t%zu\t\t%zu\t\t%f\t%f\n", i, list1[i], list2[i], lminrs[i], coords1->xSrc[i]);
    fclose(fOut);

    return 0;
}
#endif
#ifdef HACK
    vec<size_t> list1(sz[0]);
    vec<size_t> list2(sz[0]);
    vec<geom_t> lminrs(sz[0]);
    FILE * fOut = fopen(name.c_str(), "w+");
    fread(list1.data(), list1.size(), sizeof(size_t), fOut);
    fread(list2.data(), list2.size(), sizeof(size_t), fOut);
    fread(lminrs.data(), lminrs.size(), sizeof(geom_t), fOut);
    fclose(fOut);
#endif

    //free up some memory
    coords1.release();
    coords2.release();

/*****************************************************************************/
/********************/ cmsg(piol, "Output phase"); /**************************/
/*****************************************************************************/
    //Enable as many of the parameters as possible
    auto rule = std::make_shared<Rule>(true, true, true);
    rule->addFloat(Meta::dsdr, Tr::SrcMeas, Tr::SrcMeasExp);

    time = MPI_Wtime();
    //Open and write out file1 --> file3
    File::Direct file3(piol, name3, FileMode::Write);

    cmsg(piol, "Output 3");
    //select(piol, rule, file3, file1, list1, minrs);
    #warning the wrong but safer function
    selectDupe(piol, rule, file3, file1, list1, lminrs);

    cmsg(piol, "Output 3 in " + std::to_string(MPI_Wtime()- time) + " seconds");
    time = MPI_Wtime();
    //Open and write out file2 --> file4
    //This case is more complicated because the list is unordered and there can be duplicate entries
    //in the list.
    File::Direct file4(piol, name4, FileMode::Write);
    selectDupe(piol, rule, file4, file2, list2, lminrs);
    cmsg(piol, "Output 4 in " + std::to_string(MPI_Wtime()- time) + " seconds");

    return 0;
}
