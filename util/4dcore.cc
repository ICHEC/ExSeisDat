/*******************************************************************************************//*!
 *   @file
 *   @author Cathal O Broin - cathal@ichec.ie - first commit
 *   @copyright TBD. Do not distribute
 *   @date March 2017
 *   @brief This file contains the compute heavy kernel of the 4d-binning utility
*//*******************************************************************************************/
#include "4dcore.hh"
#include "share/mpi.hh"
#include <algorithm>
#include <assert.h>
#include <math.h>

namespace PIOL {
namespace FOURD {

//This function prints to stdio
/*! Process 0 will print the xSrc min and max values for each process
 *  @param[in] piol The piol object.
 *  @param[in] xsmin A vector of the min xSrc for each process.
 *  @param[in] xsmax A vector of the max xSrc for each process.
 */
void printxSrcMinMax(ExSeisPIOL* piol, vec<fourd_t>& xsmin, vec<fourd_t>& xsmax)
{
    piol->comm->barrier();
    assert(xsmin.size() == xsmax.size());
    if (!piol->comm->getRank())
        for (size_t i = 0; i < xsmin.size(); i++)
            std::cout << "minmax " << i << " " << xsmin[i] << " " << xsmax[i]
                      << std::endl;
    piol->comm->barrier();
}

/*! Each process will print their xSrc min/max and active ranks to an output file.
 *  xSrc min/max will also be printed to the terminal.
 * @param[in] piol The piol object.
 * @param[in] xslmin The minimum local value for xSrc from file 1.
 * @param[in] xslmax The maximum local value for xSrc from file 1.
 * @param[in] xsrmin The minimum local value for xSrc from file 2.
 * @param[in] xsrmax The maximum local value for xSrc from file 2.
 * @param[in] active An array of active ranks for the local process. That is, processes the
 *            local process will do a one-sided MPI_Get on.
 */
void printxSMinMax(
  ExSeisPIOL* piol,
  fourd_t xslmin,
  fourd_t xslmax,
  fourd_t xsrmin,
  fourd_t xsrmax,
  vec<size_t>& active)
{
    size_t rank      = piol->comm->getRank();
    std::string name = "tmp/temp" + std::to_string(rank);
    FILE* fOut       = fopen(name.c_str(), "w+");
    fprintf(fOut, "1-xsmin/max %f %f\n", xslmin, xslmax);
    fprintf(fOut, "2-xsmin/max %f %f\n", xsrmin, xsrmax);
    for (size_t i = 0; i < active.size(); i++)
        fprintf(fOut, "%zu\n", active[i]);
    fclose(fOut);

    auto lxsmin = piol->comm->gather(vec<fourd_t>{xslmin});
    auto lxsmax = piol->comm->gather(vec<fourd_t>{xslmax});
    printxSrcMinMax(piol, lxsmin, lxsmax);
    if (!rank) std::cout << "file2 min/max\n";
}

/*! Calculate the hypotenuse of a RH triangle using x and y as lengths.
 *  @param[in] x One side.
 *  @param[in] y The other.
 *  \note The intel compiler uses an incorrect hypotenuse function when it vectorises, so this
 *        is defined (intel 2016).
 *  @return Return the hypotenuse.
 */
fourd_t hypot(const fourd_t x, const fourd_t y)
{
    return sqrtf(x * x + y * y);
    //    return std::hypot(x, y);
}

/*! Create windows for one-sided communication of coordinates
 *  @param[in] crd The coordinate structure of arrays to open to RDMA.
 *  @param[in] ixline The inline/crossline are included
 *  @return Return a vector of all the window values.
 */
vec<MPI_Win> createCoordsWin(const Coords* crd, const bool ixline)
{
    vec<MPI_Win> win(5);
    MPI_Info info;
    MPI_Info_create(&info);
    MPI_Info_set(info, "same_disp_unit", "true");
    MPI_Info_set(info, "accumulate_ops", "same_op");

    //Look at MPI_Info
    MPIErr(MPI_Win_create(
      crd->xSrc, crd->sz, sizeof(fourd_t), info, MPI_COMM_WORLD, &win[0]));
    MPIErr(MPI_Win_create(
      crd->ySrc, crd->sz, sizeof(fourd_t), info, MPI_COMM_WORLD, &win[1]));
    MPIErr(MPI_Win_create(
      crd->xRcv, crd->sz, sizeof(fourd_t), info, MPI_COMM_WORLD, &win[2]));
    MPIErr(MPI_Win_create(
      crd->yRcv, crd->sz, sizeof(fourd_t), info, MPI_COMM_WORLD, &win[3]));
    MPIErr(MPI_Win_create(
      crd->tn, crd->sz, sizeof(size_t), info, MPI_COMM_WORLD, &win[4]));

    if (ixline) {
        win.resize(7);
        MPIErr(MPI_Win_create(
          crd->il, crd->sz, sizeof(llint), info, MPI_COMM_WORLD, &win[5]));
        MPIErr(MPI_Win_create(
          crd->xl, crd->sz, sizeof(llint), info, MPI_COMM_WORLD, &win[6]));
    }

    MPI_Info_free(&info);
    return win;
}

/*! One-sided retrieval from data in window on processor lrank. Processor lrank is passive.
 *  @param[in] lrank The rank
 *  @param[in] sz The number of coordinates
 *  @param[in] win The vector of windows to access with.
 *  @param[in] ixline The inline/crossline are included
 *  @return Return the associated Coords structure
 */
std::unique_ptr<Coords> getCoordsWin(
  size_t lrank, size_t sz, vec<MPI_Win>& win, bool ixline)
{
    auto crd = std::make_unique<Coords>(sz, ixline);
    for (size_t i = 0; i < win.size(); i++)
        MPIErr(MPI_Win_lock(MPI_LOCK_SHARED, lrank, MPI_MODE_NOCHECK, win[i]));

    MPIErr(MPI_Get(
      crd->xSrc, crd->sz, MPIType<fourd_t>(), lrank, 0, sz, MPIType<fourd_t>(),
      win[0]));
    MPIErr(MPI_Get(
      crd->ySrc, crd->sz, MPIType<fourd_t>(), lrank, 0, sz, MPIType<fourd_t>(),
      win[1]));
    MPIErr(MPI_Get(
      crd->xRcv, crd->sz, MPIType<fourd_t>(), lrank, 0, sz, MPIType<fourd_t>(),
      win[2]));
    MPIErr(MPI_Get(
      crd->yRcv, crd->sz, MPIType<fourd_t>(), lrank, 0, sz, MPIType<fourd_t>(),
      win[3]));
    MPIErr(MPI_Get(
      crd->tn, crd->sz, MPIType<size_t>(), lrank, 0, sz, MPIType<size_t>(),
      win[4]));

    if (ixline) {
        MPIErr(MPI_Get(
          crd->il, crd->sz, MPIType<llint>(), lrank, 0, sz, MPIType<llint>(),
          win[5]));
        MPIErr(MPI_Get(
          crd->xl, crd->sz, MPIType<llint>(), lrank, 0, sz, MPIType<llint>(),
          win[6]));
    }

    for (size_t i = 0; i < win.size(); i++)
        MPIErr(MPI_Win_unlock(lrank, win[i]));

    return crd;
}

/*! Calcuate the difference criteria between source/receiver pairs between traces.
 *  @param[in] xs1 xSrc from pair 1
 *  @param[in] ys1 ySrc from pair 1
 *  @param[in] xr1 xRcv from pair 1
 *  @param[in] yr1 yRcv from pair 1
 *  @param[in] xs2 xSrc from pair 2
 *  @param[in] ys2 ySrc from pair 2
 *  @param[in] xr2 xRcv from pair 2
 *  @param[in] yr2 yRcv from pair 2
 *  @return Return the dsr value
 *
 * @details The definitions of ds and dr are:
 * \f$ds = \sqrt((sx1-sx2)^2+(sy1-sy2)^2)\f$
 * \f$dr = \sqrt((rx1-rx2)^2+(ry1-ry2)^2)\f$
 * The boat may be going in the opposite direction for the data in file2 so that the source from file1
 * is closer to the receiver in file2 and vice-versa. We interchange _sx->rx, _sy->ry _rx->sx _ry->sy
 * for prm2 in that case (prm2).
 * \f$rds = \sqrt((sx1-rx2)^2+(sy1-ry2)^2)\f$
 * \f$rdr = \sqrt((rx1-sx2)^2+(ry1-sy2)^2)\f$
 * \f$dsr = (min(ds, rds) + min(dr, rdr))^2\f$
*/
fourd_t dsr(
  const fourd_t xs1,
  const fourd_t ys1,
  const fourd_t xr1,
  const fourd_t yr1,
  const fourd_t xs2,
  const fourd_t ys2,
  const fourd_t xr2,
  const fourd_t yr2)
{
    const fourd_t forward = hypot(xs1 - xs2, ys1 - ys2)
                            + hypot(xr1 - xr2, yr1 - yr2);  //Forward-boat case
    const fourd_t reverse = hypot(xs1 - xr2, ys1 - yr2)
                            + hypot(xr1 - xs2, yr1 - ys2);  //Reverse-boat case
    return std::min(forward, reverse);
}

/*! Perform a minimisation check with the current two vectors of parameters.
 *  @tparam Init If true, perform the initialisation sequence.
 *  @param[in] crd1 A vector containing the process's parameter data from the first input file. This data
 *             is never sent to any crd2 process.
 *  @param[in] crd2 A vector containing the parameter data from another process.
 *  @param[out] min A vector containing the trace number of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 *  @param[out] minrs A vector containing the dsdr value of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 */
template<const bool ixline>
void initUpdate(
  const Coords* crd1, const Coords* crd2, vec<size_t>& min, vec<fourd_t>& minrs)
{
    for (size_t i = 0; i < crd1->sz; i++) {
        minrs[i] =
          (!ixline
               || (crd1->il[i] == crd2->il[0] && crd1->xl[i] == crd2->xl[0]) ?
             dsr(
               crd1->xSrc[i], crd1->ySrc[i], crd1->xRcv[i], crd1->yRcv[i],
               crd2->xSrc[0], crd2->ySrc[0], crd2->xRcv[0], crd2->yRcv[0]) :
             std::numeric_limits<fourd_t>::max());
        min[i] = crd2->tn[0];
    }
}

/*! Perform a minimisation check with the current two vectors of parameters.
 *  @tparam Init If true, perform the initialisation sequence.
 *  @param[in] crd1 A vector containing the process's parameter data from the first input file. This data
 *             is never sent to any other process.
 *  @param[in] crd2 A vector containing the parameter data from another process.
 *  @param[in,out] min A vector containing the trace number of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 *  @param[in,out] minrs A vector containing the dsdr value of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 *  @param[in] dsrmax The maximum distance a pair from crd2 can be from crd1.
 *  @return Return the number of dsr calculations performed.
 */
template<const bool ixline>
size_t update(
  const Coords* crd1,
  const Coords* crd2,
  vec<size_t>& min,
  vec<fourd_t>& minrs,
  const fourd_t dsrmax)
{
    //For the vectorisation
    size_t lstart = 0LU;
    size_t lend   = crd1->sz;
    size_t rstart = 0LU;
    size_t rend   = crd2->sz;

    //Ignore all file2 traces that can not possibly match our criteria within the min/max
    //of src x.
    for (; rstart < rend && crd2->xSrc[rstart] < crd1->xSrc[lstart] - dsrmax;
         rstart++)
        ;
    for (; rend >= rstart && crd2->xSrc[rend] > crd1->xSrc[lend - 1] + dsrmax;
         rend--)
        ;
    for (; lstart < lend && crd1->xSrc[lstart] < crd2->xSrc[rstart] - dsrmax;
         lstart++)
        ;
    for (; lend >= lstart && crd1->xSrc[lend] > crd2->xSrc[rend - 1] + dsrmax;
         lend--)
        ;

    //TODO: Check if theoretical speedup is realisable for this alignment
    lstart     = size_t(lstart / ALIGN) * ALIGN;
    size_t lsz = lend - lstart;
    size_t rsz = rend - rstart;

    //Copy min and minrs to aligned memory
    fourd_t* lminrs;
    size_t* lmin;
    posix_memalign(
      reinterpret_cast<void**>(&lminrs), ALIGN, crd1->sz * sizeof(fourd_t));
    posix_memalign(
      reinterpret_cast<void**>(&lmin), ALIGN, crd1->sz * sizeof(size_t));
    std::copy(min.begin(), min.begin() + crd1->sz, lmin);
    std::copy(minrs.begin(), minrs.begin() + crd1->sz, lminrs);

    //These declarations are so that the compiler handles the pragma correctly
    const fourd_t* xS1 = crd1->xSrc;
    const fourd_t* xR1 = crd1->xRcv;
    const fourd_t* yS1 = crd1->ySrc;
    const fourd_t* yR1 = crd1->yRcv;
    const fourd_t* xS2 = crd2->xSrc;
    const fourd_t* xR2 = crd2->xRcv;
    const fourd_t* yS2 = crd2->ySrc;
    const fourd_t* yR2 = crd2->yRcv;
    const size_t* tn   = crd2->tn;

    #pragma omp simd aligned(xS2:ALIGN) aligned(yS2:ALIGN) aligned(xR2:ALIGN) aligned(yR2:ALIGN) \
                     aligned(xS1:ALIGN) aligned(yS1:ALIGN) aligned(xR1:ALIGN) aligned(yR1:ALIGN) \
                     aligned(lminrs:ALIGN) aligned(lmin:ALIGN) aligned(tn:ALIGN)
    for (size_t i = lstart; i < lend; i++)  //Loop through every file1 trace
    {
        const fourd_t xs1 = xS1[i], ys1 = yS1[i], xr1 = xR1[i], yr1 = yR1[i];
        size_t lm    = lmin[i];
        fourd_t lmrs = lminrs[i];
        for (size_t j = rstart; j < rend;
             j++)  //loop through a multiple of the alignment
        {
            const fourd_t xs2 = xS2[j], ys2 = yS2[j], xr2 = xR2[j],
                          yr2 = yR2[j];
            fourd_t dval =
              (!ixline
                   || (crd1->il[i] == crd2->il[j]
                       && crd1->xl[i] == crd2->xl[j]) ?
                 dsr(xs1, ys1, xr1, yr1, xs2, ys2, xr2, yr2) :
                 std::numeric_limits<fourd_t>::max());
            lm   = (dval < lmrs ? tn[j] : lm);  //Update min if applicable
            lmrs = std::min(dval, lmrs);        //Update minrs if applicable
        }
        lmin[i]   = lm;
        lminrs[i] = lmrs;
    }

    std::copy(lmin, lmin + crd1->sz, min.begin());
    std::copy(lminrs, lminrs + crd1->sz, minrs.begin());
    free(lmin);
    free(lminrs);
    return lsz * rsz;
}

/*! Send coordinate data. An alternative but largely untested alternative
 *  to the one-sided communication mechanism.
 *  @param[in] lrank The target rank to communicate with
 *  @param[in] crd The coordinate data
 *  @param[in] ixline Whether line numbers are also being sent
 *  @return A vector of MPI_Request objects for performing a MPI_Waitall
 */
vec<MPI_Request> sendCrd(size_t lrank, const Coords* crd, bool ixline)
{
    vec<MPI_Request> request(5);
    MPIErr(MPI_Isend(
      crd->xSrc, crd->sz, MPIType<fourd_t>(), lrank, 0, MPI_COMM_WORLD,
      &request[0]));
    MPIErr(MPI_Isend(
      crd->ySrc, crd->sz, MPIType<fourd_t>(), lrank, 1, MPI_COMM_WORLD,
      &request[1]));
    MPIErr(MPI_Isend(
      crd->xRcv, crd->sz, MPIType<fourd_t>(), lrank, 2, MPI_COMM_WORLD,
      &request[2]));
    MPIErr(MPI_Isend(
      crd->yRcv, crd->sz, MPIType<fourd_t>(), lrank, 3, MPI_COMM_WORLD,
      &request[3]));
    MPIErr(MPI_Isend(
      crd->tn, crd->sz, MPIType<size_t>(), lrank, 4, MPI_COMM_WORLD,
      &request[4]));

    if (ixline) {
        request.resize(7);
        MPIErr(MPI_Isend(
          crd->il, crd->sz, MPIType<llint>(), lrank, 5, MPI_COMM_WORLD,
          &request[5]));
        MPIErr(MPI_Isend(
          crd->xl, crd->sz, MPIType<llint>(), lrank, 6, MPI_COMM_WORLD,
          &request[6]));
    }
    return request;
}

/*! Reive coordinate data. An alternative but largely untested alternative
 *  to the one-sided communication mechanism.
 *  @param[in] lrank The target rank to communicate with
 *  @param[in] sz The number of entries to recv
 *  @param[in] ixline Whether line numbers are also being received
 *  @return The coordinate data
 */
std::unique_ptr<Coords> recvCrd(size_t lrank, size_t sz, bool ixline)
{
    auto crd = std::make_unique<Coords>(sz, ixline);
    vec<MPI_Request> request(5);
    MPIErr(MPI_Irecv(
      crd->xSrc, crd->sz, MPIType<fourd_t>(), lrank, 0, MPI_COMM_WORLD,
      &request[0]));
    MPIErr(MPI_Irecv(
      crd->ySrc, crd->sz, MPIType<fourd_t>(), lrank, 1, MPI_COMM_WORLD,
      &request[1]));
    MPIErr(MPI_Irecv(
      crd->xRcv, crd->sz, MPIType<fourd_t>(), lrank, 2, MPI_COMM_WORLD,
      &request[2]));
    MPIErr(MPI_Irecv(
      crd->yRcv, crd->sz, MPIType<fourd_t>(), lrank, 3, MPI_COMM_WORLD,
      &request[3]));
    MPIErr(MPI_Irecv(
      crd->tn, crd->sz, MPIType<size_t>(), lrank, 4, MPI_COMM_WORLD,
      &request[4]));

    if (ixline) {
        request.resize(7);
        MPIErr(MPI_Irecv(
          crd->il, crd->sz, MPIType<llint>(), lrank, 5, MPI_COMM_WORLD,
          &request[5]));
        MPIErr(MPI_Irecv(
          crd->xl, crd->sz, MPIType<llint>(), lrank, 6, MPI_COMM_WORLD,
          &request[6]));
    }
    vec<MPI_Status> stat(request.size());
    MPIErr(MPI_Waitall(request.size(), request.data(), stat.data()));

    return crd;
}

void calc4DBin(
  ExSeisPIOL* piol,
  const fourd_t dsrmax,
  const Coords* crd1,
  const Coords* crd2,
  const FourDOpt opt,
  vec<size_t>& min,
  vec<fourd_t>& minrs)
{
    cmsg(piol, "Compute phase");
    size_t rank    = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();
    auto szall     = piol->comm->gather(vec<size_t>{crd2->sz});

    //The File2 min/max from every process
    auto xsmin = piol->comm->gather(vec<fourd_t>{crd2->xSrc[0LU]});
    auto xsmax = piol->comm->gather(vec<fourd_t>{crd2->xSrc[crd2->sz - 1LU]});

    //The File1 local min and local maximum for the particular process
    auto xslmin = crd1->xSrc[0LU];
    auto xslmax = crd1->xSrc[crd1->sz - 1LU];

    //Perform a local initialisation update of min and minrs
    if (opt.ixline)
        initUpdate<true>(crd1, crd2, min, minrs);
    else
        initUpdate<false>(crd1, crd2, min, minrs);

    //This for loop determines the processes the local process will need to be communicating with.
    vec<size_t> active;
    for (size_t i = 0LU; i < numRank; i++)
        if ((xsmin[i] - dsrmax <= xslmax) && (xsmax[i] + dsrmax >= xslmin))
            active.push_back(i);

    if (opt.verbose) {
        printxSMinMax(piol, xslmin, xslmax, xsmin[rank], xsmax[rank], active);
        printxSrcMinMax(piol, xsmin, xsmax);
    }

    auto time = MPI_Wtime();
#define ONE_WAY_COMM
#ifdef ONE_WAY_COMM
    auto win = createCoordsWin(crd2, opt.ixline);
#else
    vec<MPI_Request> reqs;
    {
        auto xsfmin = piol->comm->gather<fourd_t>(xslmin);
        auto xsfmax = piol->comm->gather<fourd_t>(xslmax);

        for (size_t i = 0LU; i < numRank; i++)
            if (
              (xsmin[rank] - dsrmax <= xsfmax[i])
              && (xsmax[rank] + dsrmax >= xsfmin[i])) {
                vec<MPI_Request> rq = sendCrd(i, crd1, opt.ixline);
                reqs.insert(reqs.end(), rq.begin(), rq.end());
            }
    }
    piol->comm->barrier();
#endif
    //Load balancing would make sense since the workloads are non-symmetric.
    //It might be difficult to do load-balancing though considering workload
    //very much depends on the constraints from both processess

    //Perform the updates of min and minrs using data from other processes.
    //This is the main loop.
    for (size_t i = 0; i < active.size(); i++) {
        double ltime = MPI_Wtime();
        size_t lrank = active[i];
#ifdef ONE_WAY_COMM
        auto proc = getCoordsWin(lrank, szall[lrank], win, opt.ixline);
#else
        auto proc = recvCrd(lrank, szall[lrank], opt.ixline);
#endif
        double wtime = MPI_Wtime() - ltime;
        double sent =
          szall[lrank]
          * (4LU * sizeof(fourd_t) + sizeof(size_t) + 2LU * sizeof(llint));
        size_t ops =
          (opt.ixline ? update<true>(crd1, proc.get(), min, minrs, dsrmax) :
                        update<false>(crd1, proc.get(), min, minrs, dsrmax));

        std::cout << rank << "\t-->\t" << active[i]
                  << "\tTime: " << MPI_Wtime() - ltime - wtime << " sec."
                  << " Comm: " << wtime << " sec "
                  << sent / wtime / pow(2., 30.) << " GB/s. OpSave: "
                  << (double(crd1->sz * proc->sz - ops))
                       / double(crd1->sz * proc->sz) * 100.0
                  << std::endl;
    }

    std::cout << rank << " done. "
              << "Total rounds: " << active.size()
              << " Time: " << MPI_Wtime() - time << " seconds" << std::endl;

#ifdef ONE_WAY_COMM
    for (size_t i = 0; i < win.size(); i++)
        MPIErr(MPI_Win_free(&win[i]));
#else

    vec<MPI_Status> stat(reqs.size());
    MPIErr(MPI_Waitall(reqs.size(), reqs.data(), stat.data()));

#endif
    piol->comm->barrier();
    cmsg(
      piol, "Compute phase completed in " + std::to_string(MPI_Wtime() - time)
              + " seconds");
}
}  // namespace FOURD
}  // namespace PIOL
