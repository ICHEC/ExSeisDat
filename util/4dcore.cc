#include <assert.h>
#include <algorithm>
#include "share/mpi.hh"
#include "4dcore.hh"

namespace PIOL { namespace FOURD {

//This function prints to stdio
void printxSrcMinMax(ExSeisPIOL * piol, vec<geom_t> & xsmin, vec<geom_t> & xsmax)
{
    piol->comm->barrier();
    assert(xsmin.size() == xsmax.size());
    if (!piol->comm->getRank())
        for (size_t i = 0; i < xsmin.size(); i++)
            std::cout << "minmax " << i << " " << xsmin[i] << " " << xsmax[i] << std::endl;
    piol->comm->barrier();
}

//This function outputs some data for inspection
void printxSMinMax(ExSeisPIOL * piol, geom_t xslmin, geom_t xslmax, geom_t xsrmin, geom_t xsrmax, vec<size_t> & active)
{
    size_t rank = piol->comm->getRank();
    std::string name = "tmp/temp" + std::to_string(rank);
    FILE * fOut = fopen(name.c_str(), "w+");
    fprintf(fOut, "1-xsmin/max %f %f\n", xslmin, xslmax);
    fprintf(fOut, "2-xsmin/max %f %f\n", xsrmin, xsrmax);
    for (size_t i = 0; i < active.size(); i++)
        fprintf(fOut, "%zu\n", active[i]);
    fclose(fOut);
    auto lxsmin = piol->comm->gather(vec<geom_t>{xslmin});
    auto lxsmax = piol->comm->gather(vec<geom_t>{xslmax});
    printxSrcMinMax(piol, lxsmin, lxsmax);
    if (!rank)
        std::cout << "file2 min/max\n";
}

/* The intel compiler uses an incorrect hypotenuse function when it vectorises.
 */
geom_t hypot(geom_t x, geom_t y)
{
    return sqrtf(x*x + y*y);
}

/*! Create windows for one-sided communication of coordinates
 *  \param[in] coords The coordinate structure of arrays to open to RDMA.
 */
vec<MPI_Win> createCoordsWindow(const Coords * coords)
{
    vec<MPI_Win> win(5);
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
#warning I don't think this is correct yet
    for (size_t i = 0; i < 5; i++)
        MPI_Win_lock(MPI_LOCK_SHARED, lrank, MPI_MODE_NOCHECK, win[i]);
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

/*! Calcuate the difference criteria between source/receiver pairs between traces. For each trace in
 *  file1 the fourdbin utility finds the trace from file2 which is the minimum distance away.
 *  \param[in] prm1 A pointer to an array of size 4 which has sx, sy, rx, ry in that order. (file1
 *  \param[in] prm2 A pointer to an array of size 4 which has sx, sy, rx, ry in that order. (file2)
 *  \return Return the dsr value.
 *
 * The definitions of ds and dr are:
 * ds = \sqrt((sx1-sx2)^2+(sy1-sy2)^2)
 * dr = \sqrt((rx1-rx2)^2+(ry1-ry2)^2)
 * The boat may be going in the opposite direction for the data in file2 so that the source from file1
 * is closer to the receiver in file2 and vice-versa. We interchange _sx->rx, _sy->ry _rx->sx _ry->sy
 * for prm2 in that case (prm2).
 * rds = \sqrt((sx1-rx2)^2+(sy1-ry2)^2)
 * rdr = \sqrt((rx1-sx2)^2+(ry1-sy2)^2)
 * dsr = (min(ds, rds) + min(dr, rdr))^2
 * \todo Compute load just jumped up. No Branching. Acceleration opportunity! */
geom_t dsr(const geom_t xs1, const geom_t ys1, const geom_t xr1, const geom_t yr1,
           const geom_t xs2, const geom_t ys2, const geom_t xr2, const geom_t yr2)
{
    const geom_t forward = hypot(xs1 - xs2, ys1 - ys2) + hypot(xr1 - xr2, yr1 - yr2); //Forward-boat case
    const geom_t reverse = hypot(xs1 - xr2, ys1 - yr2) + hypot(xr1 - xs2, yr1 - ys2); //Reverse-boat case
    return std::min(forward, reverse);
}

/*! Perform a minimisation check with the current two vectors of parameters.
 *  \tparam Init If true, perform the initialisation sequence.
 *  \param[in] szall A vector containing the amount of data each process has from the second input file.
 *  \param[in] crd1 A vector containing the process's parameter data from the first input file. This data
 *             is never sent to any crd2 process.
 *  \param[in] crd2 A vector containing the parameter data from another process.
 *  \param[in,out] min A vector containing the trace number of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 *  \param[in,out] minrs A vector containing the dsdr value of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 */
void initUpdate(size_t offset, const Coords * crd1, const Coords * crd2, vec<size_t> & min, vec<geom_t> & minrs)
{
    for (size_t i = 0; i < crd1->sz; i++)
    {
        minrs[i] = dsr(crd1->xSrc[i], crd1->ySrc[i], crd1->xRcv[i], crd1->yRcv[i],
                       crd2->xSrc[0], crd2->ySrc[0], crd2->xRcv[0], crd2->yRcv[0]);
        min[i] = offset;
    }
}

/*! Perform a minimisation check with the current two vectors of parameters.
 *  \tparam Init If true, perform the initialisation sequence.
 *  \param[in] szall A vector containing the amount of data each process has from the second input file.
 *  \param[in] crd1 A vector containing the process's parameter data from the first input file. This data
 *             is never sent to any other process.
 *  \param[in] crd2 A vector containing the parameter data from another process.
 *  \param[in,out] min A vector containing the trace number of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 *  \param[in,out] minrs A vector containing the dsdr value of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 */
size_t update(size_t rank, size_t offset, const Coords * crd1, const Coords * crd2, vec<size_t> & min, vec<geom_t> & minrs, geom_t dsrmax)
{
    size_t sz = crd1->sz;

    //For the vectorisation
    size_t lstart = 0U;
    size_t lend = crd1->sz;
    size_t rstart = 0U;
    size_t rend = crd2->sz;

    const geom_t * lxS = crd1->xSrc;
    const geom_t * lyS = crd1->ySrc;
    const geom_t * lxR = crd1->xRcv;
    const geom_t * lyR = crd1->yRcv;
    const geom_t * rxS = crd2->xSrc;
    const geom_t * ryS = crd2->ySrc;
    const geom_t * rxR = crd2->xRcv;
    const geom_t * ryR = crd2->yRcv;
    const size_t * tn = crd2->tn;

    //Ignore all file2 traces that can not possibly match our criteria within the min/max
    //of src x.
    while (rstart < crd2->sz && rxS[rstart] < lxS[0] - dsrmax)
        rstart++;
    while (rend >= rstart && rxS[rend] > lxS[sz-1] + dsrmax)
        rend--;
    while (lstart < sz && lxS[lstart] < rxS[rstart] - dsrmax)
        lstart++;
    while (lend >= lstart && lxS[lend] > rxS[rend-1] + dsrmax)
        lend--;

    //TODO: A more advanced reduction of the workload would be to:
    //  sort by src-x --> drop traces
    //  sort by src-y --> drop traces
    //  sort by rcv-x --> drop traces
    //  sort by rcv-y --> drop traces

    //TODO: Check if theoretical speedup is realisable for this alignment
    lstart = size_t(lstart / ALIGN) * ALIGN;

    //TODO: if I want to use this next line, I need to resize lmin etc.

    size_t lsz = lend-lstart;
    size_t rsz = rend-rstart;
    size_t lszAlign = size_t((lend + ALIGN-1U)/ ALIGN) * ALIGN - lstart;

    if (!lsz || !rsz)
        return 0U;

    //Copy min and minrs to aligned memory
    geom_t * lminrs;
    size_t * lmin;
    posix_memalign(reinterpret_cast<void **>(&lminrs), ALIGN, lszAlign * sizeof(geom_t));
    posix_memalign(reinterpret_cast<void **>(&lmin), ALIGN, lszAlign * sizeof(size_t));
    std::copy(min.begin(), min.begin() + lsz, lmin);
    std::copy(minrs.begin(), minrs.begin() + lsz, lminrs);

    lxS = &crd1->xSrc[lstart];
    lyS = &crd1->ySrc[lstart];
    lxR = &crd1->xRcv[lstart];
    lyR = &crd1->yRcv[lstart];
    rxS = &crd2->xSrc[rstart];
    ryS = &crd2->ySrc[rstart];
    rxR = &crd2->xRcv[rstart];
    ryR = &crd2->yRcv[rstart];
    tn = &crd2->tn[rstart];

    #pragma omp simd aligned(rxS:ALIGN) aligned(ryS:ALIGN) aligned(rxR:ALIGN) aligned(ryR:ALIGN) \
                     aligned(lxS:ALIGN) aligned(lyS:ALIGN) aligned(lxR:ALIGN) aligned(lyR:ALIGN) \
                     aligned(lminrs:ALIGN) aligned(lmin:ALIGN) aligned(tn:ALIGN)
    for (size_t i = 0U; i < lszAlign; i++)                         //Loop through every file1 trace
    {
        const geom_t lxs = lxS[i], lys = lyS[i], lxr = lxR[i], lyr = lyR[i];
        size_t lm = lmin[i];
        geom_t lmrs = lminrs[i];
        for (size_t j = 0U; j < rsz; j++)        //loop through a multiple of the alignment
        {
            const geom_t rxs = rxS[j], rys = ryS[j], rxr = rxR[j], ryr = ryR[j];
            geom_t dval = dsr(lxs, lys, lxr, lyr, rxs, rys, rxr, ryr);
            lm = (dval < lmrs ? tn[j] : lm);      //Update min if applicable
            lmrs = std::min(dval, lmrs);          //Update minrs if applicable
        }
        lmin[i] = lm;
        lminrs[i] = lmrs;
    }

    std::copy(lmin, lmin+lsz, min.begin() + lstart);
    std::copy(lminrs, lminrs+lsz, minrs.begin() + lstart);
    free(lmin);
    free(lminrs);
    return lsz * rsz;
}

void calc4DBin(ExSeisPIOL * piol, const geom_t dsrmax, const Coords * crd1, const Coords * coords2, vec<size_t> & min, vec<geom_t> & minrs, bool verbose)
{
    cmsg(piol, "Compute phase");
    size_t rank = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();
    auto szall = piol->comm->gather(vec<size_t>{coords2->sz});
    vec<size_t> offset(szall.size());
    for (size_t i = 1; i < offset.size(); i++)
        offset[i] = offset[i-1] + szall[i];

    //The File2 min/max from every process
    auto xsmin = piol->comm->gather(vec<geom_t>{coords2->xSrc[0U]});
    auto xsmax = piol->comm->gather(vec<geom_t>{coords2->xSrc[coords2->sz-1U]});

    //The File1 local min and local maximum for the particular process
    auto xslmin = crd1->xSrc[0U];
    auto xslmax = crd1->xSrc[crd1->sz-1U];

    //Perform a local initialisation update of min and minrs
    initUpdate(offset[rank], crd1, coords2, min, minrs);

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
        auto ops = update(rank, offset[lrank], crd1, proc.get(), min, minrs, dsrmax);
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

