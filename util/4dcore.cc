/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date March 2017
 *   \brief This file contains the compute heavy kernel of the 4d-binning utility
*//*******************************************************************************************/
#include <math.h>
#include <assert.h>
#include <algorithm>
#include "share/mpi.hh"
#include "4dcore.hh"

namespace PIOL { namespace FOURD {

//This function prints to stdio
/*! Process 0 will print the xSrc min and max values for each process
 *  \param[in] piol The piol object.
 *  \param[in] xsmin A vector of the min xSrc for each process.
 *  \param[in] xsmax A vector of the max xSrc for each process.
 */
void printxSrcMinMax(ExSeisPIOL * piol, vec<fourd_t> & xsmin, vec<fourd_t> & xsmax)
{
    piol->comm->barrier();
    assert(xsmin.size() == xsmax.size());
    if (!piol->comm->getRank())
        for (size_t i = 0; i < xsmin.size(); i++)
            std::cout << "minmax " << i << " " << xsmin[i] << " " << xsmax[i] << std::endl;
    piol->comm->barrier();
}

/*! Each process will print their xSrc min/max and active ranks to an output file.
 *  xSrc min/max will also be printed to the terminal.
 * \param[in] piol The piol object.
 * \param[in] xslmin The minimum local value for xSrc from file 1.
 * \param[in] xslmax The maximum local value for xSrc from file 1.
 * \param[in] xsrmin The minimum local value for xSrc from file 2.
 * \param[in] xsrmax The maximum local value for xSrc from file 2.
 * \param[in] active An array of active ranks for the local process. That is, processes the
 *            local process will do a one-sided MPI_Get on.
 */
void printxSMinMax(ExSeisPIOL * piol, fourd_t xslmin, fourd_t xslmax, fourd_t xsrmin, fourd_t xsrmax, vec<size_t> & active)
{
    size_t rank = piol->comm->getRank();
    std::string name = "tmp/temp" + std::to_string(rank);
    FILE * fOut = fopen(name.c_str(), "w+");
    fprintf(fOut, "1-xsmin/max %f %f\n", xslmin, xslmax);
    fprintf(fOut, "2-xsmin/max %f %f\n", xsrmin, xsrmax);
    for (size_t i = 0; i < active.size(); i++)
        fprintf(fOut, "%zu\n", active[i]);
    fclose(fOut);

    auto lxsmin = piol->comm->gather(vec<fourd_t>{xslmin});
    auto lxsmax = piol->comm->gather(vec<fourd_t>{xslmax});
    printxSrcMinMax(piol, lxsmin, lxsmax);
    if (!rank)
        std::cout << "file2 min/max\n";
}

/*! Calculate the hypotenuse of a RH triangle using x and y as lengths.
 *  \param[in] x One side.
 *  \param[in] y The other.
 *  \note The intel compiler uses an incorrect hypotenuse function when it vectorises, so this
 *        is defined.
 *  \return Return the hypotenuse.
 */
fourd_t hypot(const fourd_t x, const fourd_t y)
{
    return sqrtf(x*x + y*y);
}

/*! Create windows for one-sided communication of coordinates
 *  \param[in] coords The coordinate structure of arrays to open to RDMA.
 *  \return Return a vector of all the window values.
 */
vec<MPI_Win> createCoordsWindow(const Coords * coords)
{
    vec<MPI_Win> win(5);
    //Look at MPI_Info
    int err;
    err = MPI_Win_create(coords->xSrc, coords->sz, sizeof(fourd_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win[0]);
    assert(err == MPI_SUCCESS);
    err = MPI_Win_create(coords->ySrc, coords->sz, sizeof(fourd_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win[1]);
    assert(err == MPI_SUCCESS);
    err = MPI_Win_create(coords->xRcv, coords->sz, sizeof(fourd_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win[2]);
    assert(err == MPI_SUCCESS);
    err = MPI_Win_create(coords->yRcv, coords->sz, sizeof(fourd_t), MPI_INFO_NULL, MPI_COMM_WORLD, &win[3]);
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
    err = MPI_Get(coords->xSrc, coords->sz, MPIType<fourd_t>(), lrank, 0, sz, MPIType<fourd_t>(), win[0]);
    assert(err == MPI_SUCCESS);
    err = MPI_Get(coords->ySrc, coords->sz, MPIType<fourd_t>(), lrank, 0, sz, MPIType<fourd_t>(), win[1]);
    assert(err == MPI_SUCCESS);
    err = MPI_Get(coords->xRcv, coords->sz, MPIType<fourd_t>(), lrank, 0, sz, MPIType<fourd_t>(), win[2]);
    assert(err == MPI_SUCCESS);
    err = MPI_Get(coords->yRcv, coords->sz, MPIType<fourd_t>(), lrank, 0, sz, MPIType<fourd_t>(), win[3]);
    assert(err == MPI_SUCCESS);
    err = MPI_Get(coords->tn, coords->sz, MPIType<size_t>(), lrank, 0, sz, MPIType<size_t>(), win[4]);
    assert(err == MPI_SUCCESS);
    for (size_t i = 0; i < 5; i++)
        MPI_Win_unlock(lrank, win[i]);
    return std::move(coords);
}

/*! Calcuate the difference criteria between source/receiver pairs between traces.
 *  \param[in] xs1 xSrc from pair 1
 *  \param[in] ys1 ySrc from pair 1
 *  \param[in] xr1 xRcv from pair 1
 *  \param[in] yr1 yRcv from pair 1
 *  \param[in] xs2 xSrc from pair 2
 *  \param[in] ys2 ySrc from pair 2
 *  \param[in] xr2 xRcv from pair 2
 *  \param[in] yr2 yRcv from pair 2
 *  \return Return the dsr value
 *
 * \details The definitions of ds and dr are:
 * \f$ds = \sqrt((sx1-sx2)^2+(sy1-sy2)^2)\f$
 * \f$dr = \sqrt((rx1-rx2)^2+(ry1-ry2)^2)\f$
 * The boat may be going in the opposite direction for the data in file2 so that the source from file1
 * is closer to the receiver in file2 and vice-versa. We interchange _sx->rx, _sy->ry _rx->sx _ry->sy
 * for prm2 in that case (prm2).
 * \f$rds = \sqrt((sx1-rx2)^2+(sy1-ry2)^2)\f$
 * \f$rdr = \sqrt((rx1-sx2)^2+(ry1-sy2)^2)\f$
 * \f$dsr = (min(ds, rds) + min(dr, rdr))^2\f$
*/
fourd_t dsr(const fourd_t xs1, const fourd_t ys1, const fourd_t xr1, const fourd_t yr1,
           const fourd_t xs2, const fourd_t ys2, const fourd_t xr2, const fourd_t yr2)
{
    const fourd_t forward = hypot(xs1 - xs2, ys1 - ys2) + hypot(xr1 - xr2, yr1 - yr2); //Forward-boat case
    const fourd_t reverse = hypot(xs1 - xr2, ys1 - yr2) + hypot(xr1 - xs2, yr1 - ys2); //Reverse-boat case
    return std::min(forward, reverse);
}

/*! Perform a minimisation check with the current two vectors of parameters.
 *  \tparam Init If true, perform the initialisation sequence.
 *  \param[in] crd1 A vector containing the process's parameter data from the first input file. This data
 *             is never sent to any crd2 process.
 *  \param[in] crd2 A vector containing the parameter data from another process.
 *  \param[out] min A vector containing the trace number of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 *  \param[out] minrs A vector containing the dsdr value of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 */
void initUpdate(const Coords * crd1, const Coords * crd2, vec<size_t> & min, vec<fourd_t> & minrs)
{
    for (size_t i = 0; i < crd1->sz; i++)
    {
        minrs[i] = dsr(crd1->xSrc[i], crd1->ySrc[i], crd1->xRcv[i], crd1->yRcv[i],
                       crd2->xSrc[0], crd2->ySrc[0], crd2->xRcv[0], crd2->yRcv[0]);
        min[i] = crd2->tn[0];
    }
}

/*! Perform a minimisation check with the current two vectors of parameters.
 *  \tparam Init If true, perform the initialisation sequence.
 *  \param[in] crd1 A vector containing the process's parameter data from the first input file. This data
 *             is never sent to any other process.
 *  \param[in] crd2 A vector containing the parameter data from another process.
 *  \param[in,out] min A vector containing the trace number of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 *  \param[in,out] minrs A vector containing the dsdr value of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 *  \param[in] dsrmax The maximum distance a pair from crd2 can be from crd1.
 *  \return Return the number of dsr calculations performed.
 */
size_t update(const Coords * crd1, const Coords * crd2, vec<size_t> & min, vec<fourd_t> & minrs, const fourd_t dsrmax)
{
    //For the vectorisation
    size_t lstart = 0U;
    size_t lend = crd1->sz;
    size_t rstart = 0U;
    size_t rend = crd2->sz;

    //Ignore all file2 traces that can not possibly match our criteria within the min/max
    //of src x.
    for (;rstart < rend && crd2->xSrc[rstart] < crd1->xSrc[lstart] - dsrmax; rstart++);
    for (;rend >= rstart && crd2->xSrc[rend] > crd1->xSrc[lend-1] + dsrmax; rend--);
    for (;lstart < lend && crd1->xSrc[lstart] < crd2->xSrc[rstart] - dsrmax; lstart++);
    for (;lend >= lstart && crd1->xSrc[lend] > crd2->xSrc[rend-1] + dsrmax; lend--);

    //TODO: Check if theoretical speedup is realisable for this alignment
    lstart = size_t(lstart / ALIGN) * ALIGN;
    size_t lsz = lend-lstart;
    size_t rsz = rend-rstart;

    //Copy min and minrs to aligned memory
    fourd_t * lminrs;
    size_t * lmin;
    posix_memalign(reinterpret_cast<void **>(&lminrs), ALIGN, crd1->sz * sizeof(fourd_t));
    posix_memalign(reinterpret_cast<void **>(&lmin), ALIGN, crd1->sz * sizeof(size_t));
    std::copy(min.begin(), min.begin() + crd1->sz, lmin);
    std::copy(minrs.begin(), minrs.begin() + crd1->sz, lminrs);

    //These declarations are so that the compiler handles the pragma correctly
    const fourd_t * xS1 = crd1->xSrc;
    const fourd_t * xR1 = crd1->xRcv;
    const fourd_t * yS1 = crd1->ySrc;
    const fourd_t * yR1 = crd1->yRcv;
    const fourd_t * xS2 = crd2->xSrc;
    const fourd_t * xR2 = crd2->xRcv;
    const fourd_t * yS2 = crd2->ySrc;
    const fourd_t * yR2 = crd2->yRcv;
    const size_t * tn = crd2->tn;

    #pragma omp simd aligned(xS2:ALIGN) aligned(yS2:ALIGN) aligned(xR2:ALIGN) aligned(yR2:ALIGN) \
                     aligned(xS1:ALIGN) aligned(yS1:ALIGN) aligned(xR1:ALIGN) aligned(yR1:ALIGN) \
                     aligned(lminrs:ALIGN) aligned(lmin:ALIGN) aligned(tn:ALIGN)
    for (size_t i = lstart; i < lend; i++)                         //Loop through every file1 trace
    {
        const fourd_t xs1 = xS1[i], ys1 = yS1[i], xr1 = xR1[i], yr1 = yR1[i];
        size_t lm = lmin[i];
        fourd_t lmrs = lminrs[i];
        for (size_t j = rstart; j < rend; j++)        //loop through a multiple of the alignment
        {
            const fourd_t xs2 = xS2[j], ys2 = yS2[j], xr2 = xR2[j], yr2 = yR2[j];
            fourd_t dval = dsr(xs1, ys1, xr1, yr1, xs2, ys2, xr2, yr2);
            lm = (dval < lmrs ? tn[j] : lm);      //Update min if applicable
            lmrs = std::min(dval, lmrs);          //Update minrs if applicable
        }
        lmin[i] = lm;
        lminrs[i] = lmrs;
    }

    std::copy(lmin, lmin+crd1->sz, min.begin());
    std::copy(lminrs, lminrs+crd1->sz, minrs.begin());
    free(lmin);
    free(lminrs);
    return lsz * rsz;
}

void calc4DBin(ExSeisPIOL * piol, const fourd_t dsrmax, const Coords * crd1, const Coords * coords2, vec<size_t> & min, vec<fourd_t> & minrs, bool verbose)
{
    cmsg(piol, "Compute phase");
    size_t rank = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();
    auto szall = piol->comm->gather(vec<size_t>{coords2->sz});
    vec<size_t> offset(szall.size());
    for (size_t i = 1; i < offset.size(); i++)
        offset[i] = offset[i-1] + szall[i];

    //The File2 min/max from every process
    auto xsmin = piol->comm->gather(vec<fourd_t>{coords2->xSrc[0U]});
    auto xsmax = piol->comm->gather(vec<fourd_t>{coords2->xSrc[coords2->sz-1U]});

    //The File1 local min and local maximum for the particular process
    auto xslmin = crd1->xSrc[0U];
    auto xslmax = crd1->xSrc[crd1->sz-1U];

    //Perform a local initialisation update of min and minrs
    initUpdate(crd1, coords2, min, minrs);

    //This for loop determines the processes the local process will need to be communicating with.
    vec<size_t> active;
    for (size_t i = 0U; i < numRank; i++)
        if ((xsmin[i] - dsrmax <= xslmax) && (xsmax[i] + dsrmax >= xslmin))
            active.push_back(i);

    if (verbose)
    {
        printxSMinMax(piol, xslmin, xslmax, xsmin[rank], xsmax[rank], active);
        printxSrcMinMax(piol, xsmin, xsmax);
    }

    auto time = MPI_Wtime();
    auto win = createCoordsWindow(coords2);
    //Load balancing would make sense since the workloads are non-symmetric.
    //It might be difficult to do load-balancing though considering workload
    //very much depends on the constraints from both processess

    //Perform the updates of min and minrs using data from other processes.
    //This is the main loop.
    for (size_t i = 0; i < active.size(); i ++)
    {
        auto ltime = MPI_Wtime();
        auto lrank = active[i];
        auto proc = getCoordsWin(lrank, szall[lrank], win);
        auto wtime = MPI_Wtime() - ltime;
        auto ops = update(crd1, proc.get(), min, minrs, dsrmax);
        std::cout << rank << "\t-->\t" << active[i] << "\tTime: " << MPI_Wtime() - ltime - wtime << " sec. Comm: " << wtime << " sec. OpSave: " <<
                    (double(crd1->sz * proc->sz) - double(ops)) / double(crd1->sz * proc->sz) * 100.0 << std::endl;
    }

    std::cout << rank << " done. " << "Total rounds: " << active.size() << " Time: "<< MPI_Wtime() - time << " seconds" << std::endl;

    for (size_t i = 0; i < win.size(); i++)
    {
        int err = MPI_Win_free(&win[i]);
        assert(err == MPI_SUCCESS);
    }

    cmsg(piol, "Compute phase completed in " + std::to_string(MPI_Wtime() - time) + " seconds");
}
}}

