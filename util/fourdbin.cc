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
 */
std::unique_ptr<Coords> getCoordsWin(size_t lrank, size_t sz, vec<MPI_Win> & win)
{
    auto proc = std::make_unique<Coords>(sz);
    for (size_t i = 0; i < 5; i++)
        MPI_Win_lock(MPI_LOCK_SHARED, lrank, 0, win[i]);
    int err;
    err = MPI_Get(proc->xSrc, proc->sz, MPIType<geom_t>(), lrank, 0, sz, MPIType<geom_t>(), win[0]);
    assert(err == MPI_SUCCESS);
    err = MPI_Get(proc->ySrc, proc->sz, MPIType<geom_t>(), lrank, 0, sz, MPIType<geom_t>(), win[1]);
    assert(err == MPI_SUCCESS);
    err = MPI_Get(proc->xRcv, proc->sz, MPIType<geom_t>(), lrank, 0, sz, MPIType<geom_t>(), win[2]);
    assert(err == MPI_SUCCESS);
    err = MPI_Get(proc->yRcv, proc->sz, MPIType<geom_t>(), lrank, 0, sz, MPIType<geom_t>(), win[3]);
    assert(err == MPI_SUCCESS);
    err = MPI_Get(proc->tn, proc->sz, MPIType<size_t>(), lrank, 0, sz, MPIType<size_t>(), win[4]);
    assert(err == MPI_SUCCESS);
    for (size_t i = 0; i < 5; i++)
        MPI_Win_unlock(lrank, win[i]);
    return std::move(proc);
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

void printActivePairs(ExSeisPIOL * piol, geom_t xslmin, geom_t xslmax, vec<size_t> & active)
{
    piol->comm->barrier();
    size_t rank = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();
    for (size_t i = 0; i < numRank; i++)
    {
        if (rank == i)
            std::cout << i << " min,max = " << xslmin << " " << xslmax << std::endl;
        piol->comm->barrier();
    }
    for (size_t i = 0; i < numRank; i++)
    {
        if (rank == i)
        {
            std::cout << i << " Count " << active.size() << std::endl;
            for (size_t j = 0; j < active.size(); j++)
                std::cout << "\t active " << active[j] << std::endl;
        }
        piol->comm->barrier();
    }
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
    {
        auto dec1 = decompose(file1.readNt(), numRank, rank);
        auto dec2 = decompose(file2.readNt(), numRank, rank);

        sz[0] = dec1.second;

        coords1 = std::make_unique<Coords>(sz[0]);
        assert(coords1.get());
        getCoords(ppiol, file1, dec1.first, coords1.get());

        cmsg(piol, "Read " + std::to_string(sz[0]) + " sets of coordinates from file " + name1);

        sz[1] = dec2.second;
        coords2 = std::make_unique<Coords>(sz[1]);
        getCoords(ppiol, file2, dec2.first, coords2.get());

        cmsg(piol, "Read " + std::to_string(sz[1]) + " sets of coordinates from file " + name2);
    }

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

    //Perform a local update of min and minrs

    initUpdate(offset[rank], coords1.get(), coords2.get(), min, minrs);
    //recordTime(piol, "update 0", startTime);

    auto win = createCoordsWindow(coords2.get());

    std::vector<size_t> active;
    for (size_t i = 0U; i < numRank; i++)
    {
        //Premature optimisation?
        size_t lrank = (rank + numRank - i) % numRank;  //The rank of the left process
        if (xsmax[lrank] + dsrmax > xslmin || xsmin[lrank] + dsrmax < xslmax)
            active.push_back(lrank);
    }

    if (verbose)
    {
        printxSrcMinMax(piol, xsmin, xsmax);
        printActivePairs(piol, xslmin, xslmax, active);
    }

    //Perform the updates of min and minrs using data from other processes.
    //This is the main loop.
    for (size_t i = 0; i < active.size(); i ++)
    {
        size_t lrank = active[i];

        if (verbose)
            for (size_t j = 0; j < numRank; j++)
            {
                ppiol->comm->barrier();
                if (j == rank)
                    std::cout << j << " active " << lrank << std::endl;
                ppiol->comm->barrier();
            }

        auto proc = getCoordsWin(lrank, szall[lrank], win);

        if (verbose)
            for (size_t j = 0; j < numRank; j++)
            {
                ppiol->comm->barrier();
                if (j == rank)
                    std::cout << j << " active update " << lrank << std::endl;
                ppiol->comm->barrier();
            }

        update(offset[lrank], coords1.get(), proc.get(), min, minrs);
    }

    if (verbose)
    for (size_t i = active.size(); i < numRank; i++)
    {
        for (size_t j = 0; j < numRank; j++)
        {
            ppiol->comm->barrier();
            if (j == rank)
                std::cout << j << " inactive " << std::endl;
            ppiol->comm->barrier();
        }
        for (size_t j = 0; j < numRank; j++)
        {
            ppiol->comm->barrier();
            if (j == rank)
                std::cout << j << " inactive update" << std::endl;
            ppiol->comm->barrier();
        }
    }

    for (size_t i = 0; i < win.size(); i++)
    {
        int err = MPI_Win_free(&win[i]);
        assert(err == MPI_SUCCESS);
    }

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

    //free up some memory
    coords1.release();
    coords2.release();

    //Enable as many of the parameters as possible
    auto rule = std::make_shared<Rule>(true, true, true);
    rule->addFloat(Meta::dsdr, Tr::SrcMeas, Tr::SrcMeasExp);

/*****************************************************************************/
/********************/ cmsg(piol, "Output phase"); /**************************/
/*****************************************************************************/

    //Open and write out file1 --> file3
    File::Direct file3(piol, name3, FileMode::Write);

    cmsg(piol, "Output1");
    //select(piol, rule, file3, file1, list1, minrs);
    #warning the wrong but safer function
    selectDupe(piol, rule, file3, file1, list1, lminrs);

    //Open and write out file2 --> file4
    //This case is more complicated because the list is unordered and there can be duplicate entries
    //in the list.
    cmsg(piol, "Output2");
    File::Direct file4(piol, name4, FileMode::Write);
    selectDupe(piol, rule, file4, file2, list2, lminrs);

    return 0;
}
