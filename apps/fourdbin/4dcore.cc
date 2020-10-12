////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date March 2017
/// @brief This file contains the compute heavy kernel of the 4d-binning utility
////////////////////////////////////////////////////////////////////////////////

#include "4dcore.hh"

#include "exseis/utils/types/MPI_type.hh"

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <math.h>

namespace exseis {
namespace apps {
inline namespace fourdbin {

// This function prints to stdio
/*! Process 0 will print the x_src min and max values for each process
 *  @param[in] communicator  The communicator to print from rank 0
 *  @param[in] xsmin A vector of the min x_src for each process.
 *  @param[in] xsmax A vector of the max x_src for each process.
 */
void printx_src_min_max(
    const Communicator& communicator,
    std::vector<fourd_t>& xsmin,
    std::vector<fourd_t>& xsmax)
{
    communicator.barrier();
    assert(xsmin.size() == xsmax.size());
    if (communicator.get_rank() == 0) {
        for (size_t i = 0; i < xsmin.size(); i++) {
            std::cout << "minmax " << i << " " << xsmin[i] << " " << xsmax[i]
                      << std::endl;
        }
    }
    communicator.barrier();
}

/*! Each process will print their x_src min/max and active ranks to an output
 *  file.
 *  x_src min/max will also be printed to the terminal.
 * @param[in] communicator  The communicator to gather over
 * @param[in] xslmin The minimum local value for x_src from file 1.
 * @param[in] xslmax The maximum local value for x_src from file 1.
 * @param[in] xsrmin The minimum local value for x_src from file 2.
 * @param[in] xsrmax The maximum local value for x_src from file 2.
 * @param[in] active An array of active ranks for the local process. That is,
 *                   processes the local process will do a one-sided MPI_Get on.
 */
void print_x_src_min_max(
    const Communicator& communicator,
    fourd_t xslmin,
    fourd_t xslmax,
    fourd_t xsrmin,
    fourd_t xsrmax,
    std::vector<size_t>& active)
{
    size_t rank      = communicator.get_rank();
    std::string name = "tmp/temp" + std::to_string(rank);
    FILE* f_out      = fopen(name.c_str(), "w+");
    fprintf(f_out, "1-xsmin/max %f %f\n", xslmin, xslmax);
    fprintf(f_out, "2-xsmin/max %f %f\n", xsrmin, xsrmax);
    for (size_t i = 0; i < active.size(); i++) {
        fprintf(f_out, "%zu\n", active[i]);
    }
    fclose(f_out);

    auto lxsmin = communicator.gather(std::vector<fourd_t>{xslmin});
    auto lxsmax = communicator.gather(std::vector<fourd_t>{xslmax});
    printx_src_min_max(communicator, lxsmin, lxsmax);
    if (rank == 0) {
        std::cout << "file2 min/max\n";
    }
}

/*! Calculate the hypotenuse of a RH triangle using x and y as lengths.
 *  @param[in] x One side.
 *  @param[in] y The other.
 *  @note The intel compiler uses an incorrect hypotenuse function when it
 *        vectorises, so this is defined (intel 2016).
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
std::vector<MPI_Win> create_coords_win(const Coords* crd, const bool ixline)
{
    std::vector<MPI_Win> win(5);
    MPI_Info info;
    MPI_Info_create(&info);
    MPI_Info_set(info, "same_disp_unit", "true");
    MPI_Info_set(info, "accumulate_ops", "same_op");

    // Look at MPI_Info
    mpi_err(MPI_Win_create(
        crd->x_src, crd->sz, sizeof(fourd_t), info, MPI_COMM_WORLD, &win[0]));
    mpi_err(MPI_Win_create(
        crd->y_src, crd->sz, sizeof(fourd_t), info, MPI_COMM_WORLD, &win[1]));
    mpi_err(MPI_Win_create(
        crd->x_rcv, crd->sz, sizeof(fourd_t), info, MPI_COMM_WORLD, &win[2]));
    mpi_err(MPI_Win_create(
        crd->y_rcv, crd->sz, sizeof(fourd_t), info, MPI_COMM_WORLD, &win[3]));
    mpi_err(MPI_Win_create(
        crd->tn, crd->sz, sizeof(size_t), info, MPI_COMM_WORLD, &win[4]));

    if (ixline) {
        win.resize(7);
        mpi_err(MPI_Win_create(
            crd->il, crd->sz, sizeof(Integer), info, MPI_COMM_WORLD, &win[5]));
        mpi_err(MPI_Win_create(
            crd->xl, crd->sz, sizeof(Integer), info, MPI_COMM_WORLD, &win[6]));
    }

    MPI_Info_free(&info);
    return win;
}

/*! One-sided retrieval from data in window on processor lrank. Processor lrank
 *  is passive.
 *  @param[in] lrank The rank
 *  @param[in] sz The number of coordinates
 *  @param[in] win The vector of windows to access with.
 *  @param[in] ixline The inline/crossline are included
 *  @return Return the associated Coords structure
 */
std::unique_ptr<Coords> get_coords_win(
    size_t lrank, size_t sz, std::vector<MPI_Win>& win, bool ixline)
{
    auto crd = std::make_unique<Coords>(sz, ixline);
    for (size_t i = 0; i < win.size(); i++) {
        mpi_err(MPI_Win_lock(MPI_LOCK_SHARED, lrank, MPI_MODE_NOCHECK, win[i]));
    }

    mpi_err(MPI_Get(
        crd->x_src, crd->sz, mpi_type<fourd_t>(), lrank, 0, sz,
        mpi_type<fourd_t>(), win[0]));
    mpi_err(MPI_Get(
        crd->y_src, crd->sz, mpi_type<fourd_t>(), lrank, 0, sz,
        mpi_type<fourd_t>(), win[1]));
    mpi_err(MPI_Get(
        crd->x_rcv, crd->sz, mpi_type<fourd_t>(), lrank, 0, sz,
        mpi_type<fourd_t>(), win[2]));
    mpi_err(MPI_Get(
        crd->y_rcv, crd->sz, mpi_type<fourd_t>(), lrank, 0, sz,
        mpi_type<fourd_t>(), win[3]));
    mpi_err(MPI_Get(
        crd->tn, crd->sz, mpi_type<size_t>(), lrank, 0, sz, mpi_type<size_t>(),
        win[4]));

    if (ixline) {
        mpi_err(MPI_Get(
            crd->il, crd->sz, mpi_type<Integer>(), lrank, 0, sz,
            mpi_type<Integer>(), win[5]));
        mpi_err(MPI_Get(
            crd->xl, crd->sz, mpi_type<Integer>(), lrank, 0, sz,
            mpi_type<Integer>(), win[6]));
    }

    for (size_t i = 0; i < win.size(); i++) {
        mpi_err(MPI_Win_unlock(lrank, win[i]));
    }

    return crd;
}

/*! Calcuate the difference criteria between source/receiver pairs between
 *  traces.
 *  @param[in] xs1 x_src from pair 1
 *  @param[in] ys1 y_src from pair 1
 *  @param[in] xr1 x_rcv from pair 1
 *  @param[in] yr1 y_rcv from pair 1
 *  @param[in] xs2 x_src from pair 2
 *  @param[in] ys2 y_src from pair 2
 *  @param[in] xr2 x_rcv from pair 2
 *  @param[in] yr2 y_rcv from pair 2
 *  @return Return the dsr value
 *
 * @details The definitions of ds and dr are:
 *              \f$ds = \sqrt((sx1-sx2)^2+(sy1-sy2)^2)\f$
 *              \f$dr = \sqrt((rx1-rx2)^2+(ry1-ry2)^2)\f$
 *          The boat may be going in the opposite direction for the data in
 *          file2 so that the source from file1 is closer to the receiver in
 *          file2 and vice-versa. We interchange _sx->rx, _sy->ry, _rx->sx,
 *          _ry->sy for prm2 in that case (prm2).
 *              \f$rds = \sqrt((sx1-rx2)^2+(sy1-ry2)^2)\f$
 *              \f$rdr = \sqrt((rx1-sx2)^2+(ry1-sy2)^2)\f$
 *              \f$dsr = (min(ds, rds) + min(dr, rdr))^2\f$
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
    // Forward-boat case
    const fourd_t forward =
        hypot(xs1 - xs2, ys1 - ys2) + hypot(xr1 - xr2, yr1 - yr2);
    // Reverse-boat case
    const fourd_t reverse =
        hypot(xs1 - xr2, ys1 - yr2) + hypot(xr1 - xs2, yr1 - ys2);
    return std::min(forward, reverse);
}

/*! Perform a minimisation check with the current two vectors of parameters.
 *  @tparam Init If true, perform the initialisation sequence.
 *  @param[in] crd1 A vector containing the process's parameter data from the
 *                  first input file. This data is never sent to any crd2
 *                  process.
 *  @param[in] crd2 A vector containing the parameter data from another process.
 *  @param[out] min A vector containing the trace number of the trace that
 *                  minimises the dsdr criteria.  This vector is updated by the
 *                  loop.
 *  @param[out] minrs A vector containing the dsdr value of the trace that
 *                    minimises the dsdr criteria.  This vector is updated by
 *                    the loop.
 */
template<const bool Ixline>
void init_update(
    const Coords* crd1,
    const Coords* crd2,
    std::vector<size_t>& min,
    std::vector<fourd_t>& minrs)
{
    for (size_t i = 0; i < crd1->sz; i++) {
        minrs[i] =
            (!Ixline
                     || (crd1->il[i] == crd2->il[0]
                         && crd1->xl[i] == crd2->xl[0]) ?
                 dsr(crd1->x_src[i], crd1->y_src[i], crd1->x_rcv[i],
                     crd1->y_rcv[i], crd2->x_src[0], crd2->y_src[0],
                     crd2->x_rcv[0], crd2->y_rcv[0]) :
                 std::numeric_limits<fourd_t>::max());
        min[i] = crd2->tn[0];
    }
}

/*! Perform a minimisation check with the current two vectors of parameters.
 *  @tparam Init If true, perform the initialisation sequence.
 *  @param[in] crd1 A vector containing the process's parameter data from the
 *                  first input file. This data is never sent to any other
 *                  process.
 *  @param[in] crd2 A vector containing the parameter data from another process.
 *  @param[in,out] min A vector containing the trace number of the trace that
 *                     minimises the dsdr criteria.  This vector is updated by
 *                     the loop.
 *  @param[in,out] minrs A vector containing the dsdr value of the trace that
 *                       minimises the dsdr criteria.  This vector is updated by
 *                       the loop.
 *  @param[in] dsrmax The maximum distance a pair from crd2 can be from crd1.
 *  @return Return the number of dsr calculations performed.
 */
template<const bool Ixline>
size_t update(
    const Coords* crd1,
    const Coords* crd2,
    std::vector<size_t>& min,
    std::vector<fourd_t>& minrs,
    const fourd_t dsrmax)
{
    // For the vectorisation
    size_t lstart = 0LU;
    size_t lend   = crd1->sz;
    size_t rstart = 0LU;
    size_t rend   = crd2->sz;

    // Ignore all file2 traces that can not possibly match our criteria within
    // the min/max
    // of src x.
    for (; rstart < rend && crd2->x_src[rstart] < crd1->x_src[lstart] - dsrmax;
         rstart++) {
    }
    for (; rend >= rstart && crd2->x_src[rend] > crd1->x_src[lend - 1] + dsrmax;
         rend--) {
    }
    for (; lstart < lend && crd1->x_src[lstart] < crd2->x_src[rstart] - dsrmax;
         lstart++) {
    }
    for (; lend >= lstart && crd1->x_src[lend] > crd2->x_src[rend - 1] + dsrmax;
         lend--) {
    }

    // TODO: Check if theoretical speedup is realisable for this alignment
    lstart     = size_t(lstart / EXSEIS_ALIGN) * EXSEIS_ALIGN;
    size_t lsz = lend - lstart;
    size_t rsz = rend - rstart;

    // Copy min and minrs to aligned memory
    fourd_t* lminrs;
    size_t* lmin;
    checked_posix_memalign(
        reinterpret_cast<void**>(&lminrs), EXSEIS_ALIGN,
        crd1->sz * sizeof(fourd_t));
    checked_posix_memalign(
        reinterpret_cast<void**>(&lmin), EXSEIS_ALIGN,
        crd1->sz * sizeof(size_t));
    std::copy(min.begin(), min.begin() + crd1->sz, lmin);
    std::copy(minrs.begin(), minrs.begin() + crd1->sz, lminrs);

    // These declarations are so that the compiler handles the pragma correctly
    const fourd_t* x_s1 = crd1->x_src;
    const fourd_t* x_r1 = crd1->x_rcv;
    const fourd_t* y_s1 = crd1->y_src;
    const fourd_t* y_r1 = crd1->y_rcv;
    const fourd_t* x_s2 = crd2->x_src;
    const fourd_t* x_r2 = crd2->x_rcv;
    const fourd_t* y_s2 = crd2->y_src;
    const fourd_t* y_r2 = crd2->y_rcv;
    const size_t* tn    = crd2->tn;

    // Loop through every file1 trace
    #pragma omp simd \
        aligned(x_s1:EXSEIS_ALIGN) aligned(x_s2:EXSEIS_ALIGN) \
        aligned(x_r1:EXSEIS_ALIGN) aligned(x_r2:EXSEIS_ALIGN) \
        aligned(y_s1:EXSEIS_ALIGN) aligned(y_s2:EXSEIS_ALIGN) \
        aligned(y_r1:EXSEIS_ALIGN) aligned(y_r2:EXSEIS_ALIGN) \
        aligned(lmin:EXSEIS_ALIGN) aligned(lminrs:EXSEIS_ALIGN) \
        aligned(tn:EXSEIS_ALIGN)
    for (size_t i = lstart; i < lend; i++) {
        const fourd_t xs1 = x_s1[i];
        const fourd_t ys1 = y_s1[i];
        const fourd_t xr1 = x_r1[i];
        const fourd_t yr1 = y_r1[i];

        size_t lm    = lmin[i];
        fourd_t lmrs = lminrs[i];
        // loop through a multiple of the alignment
        for (size_t j = rstart; j < rend; j++) {
            const fourd_t xs2 = x_s2[j];
            const fourd_t ys2 = y_s2[j];
            const fourd_t xr2 = x_r2[j];
            const fourd_t yr2 = y_r2[j];

            const fourd_t dval =
                (!Ixline
                         || (crd1->il[i] == crd2->il[j]
                             && crd1->xl[i] == crd2->xl[j]) ?
                     dsr(xs1, ys1, xr1, yr1, xs2, ys2, xr2, yr2) :
                     std::numeric_limits<fourd_t>::max());

            // Update min if applicable
            lm = (dval < lmrs ? tn[j] : lm);
            // Update minrs if applicable
            lmrs = std::min(dval, lmrs);
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
std::vector<MPI_Request> send_crd(size_t lrank, const Coords* crd, bool ixline)
{
    std::vector<MPI_Request> request(5);
    mpi_err(MPI_Isend(
        crd->x_src, crd->sz, mpi_type<fourd_t>(), lrank, 0, MPI_COMM_WORLD,
        &request[0]));
    mpi_err(MPI_Isend(
        crd->y_src, crd->sz, mpi_type<fourd_t>(), lrank, 1, MPI_COMM_WORLD,
        &request[1]));
    mpi_err(MPI_Isend(
        crd->x_rcv, crd->sz, mpi_type<fourd_t>(), lrank, 2, MPI_COMM_WORLD,
        &request[2]));
    mpi_err(MPI_Isend(
        crd->y_rcv, crd->sz, mpi_type<fourd_t>(), lrank, 3, MPI_COMM_WORLD,
        &request[3]));
    mpi_err(MPI_Isend(
        crd->tn, crd->sz, mpi_type<size_t>(), lrank, 4, MPI_COMM_WORLD,
        &request[4]));

    if (ixline) {
        request.resize(7);
        mpi_err(MPI_Isend(
            crd->il, crd->sz, mpi_type<Integer>(), lrank, 5, MPI_COMM_WORLD,
            &request[5]));
        mpi_err(MPI_Isend(
            crd->xl, crd->sz, mpi_type<Integer>(), lrank, 6, MPI_COMM_WORLD,
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
std::unique_ptr<Coords> recv_crd(size_t lrank, size_t sz, bool ixline)
{
    auto crd = std::make_unique<Coords>(sz, ixline);
    std::vector<MPI_Request> request(5);
    mpi_err(MPI_Irecv(
        crd->x_src, crd->sz, mpi_type<fourd_t>(), lrank, 0, MPI_COMM_WORLD,
        &request[0]));
    mpi_err(MPI_Irecv(
        crd->y_src, crd->sz, mpi_type<fourd_t>(), lrank, 1, MPI_COMM_WORLD,
        &request[1]));
    mpi_err(MPI_Irecv(
        crd->x_rcv, crd->sz, mpi_type<fourd_t>(), lrank, 2, MPI_COMM_WORLD,
        &request[2]));
    mpi_err(MPI_Irecv(
        crd->y_rcv, crd->sz, mpi_type<fourd_t>(), lrank, 3, MPI_COMM_WORLD,
        &request[3]));
    mpi_err(MPI_Irecv(
        crd->tn, crd->sz, mpi_type<size_t>(), lrank, 4, MPI_COMM_WORLD,
        &request[4]));

    if (ixline) {
        request.resize(7);
        mpi_err(MPI_Irecv(
            crd->il, crd->sz, mpi_type<Integer>(), lrank, 5, MPI_COMM_WORLD,
            &request[5]));
        mpi_err(MPI_Irecv(
            crd->xl, crd->sz, mpi_type<Integer>(), lrank, 6, MPI_COMM_WORLD,
            &request[6]));
    }
    std::vector<MPI_Status> stat(request.size());
    mpi_err(MPI_Waitall(request.size(), request.data(), stat.data()));

    return crd;
}

void calc_4d_bin(
    const Communicator_mpi& communicator,
    fourd_t dsrmax,
    const Coords* crd1,
    const Coords* crd2,
    Four_d_opt opt,
    std::vector<size_t>& min,
    std::vector<fourd_t>& minrs)
{
    cmsg(communicator, "Compute phase");
    size_t rank     = communicator.get_rank();
    size_t num_rank = communicator.get_num_rank();
    auto szall      = communicator.gather(std::vector<size_t>{crd2->sz});

    // The File2 min/max from every process
    auto xsmin = communicator.gather(std::vector<fourd_t>{crd2->x_src[0LU]});
    auto xsmax =
        communicator.gather(std::vector<fourd_t>{crd2->x_src[crd2->sz - 1LU]});

    // The File1 local min and local maximum for the particular process
    auto xslmin = crd1->x_src[0LU];
    auto xslmax = crd1->x_src[crd1->sz - 1LU];

    // Perform a local initialisation update of min and minrs
    if (opt.ixline) {
        init_update<true>(crd1, crd2, min, minrs);
    }
    else {
        init_update<false>(crd1, crd2, min, minrs);
    }

    // This for loop determines the processes the local process will need to be
    // communicating with.
    std::vector<size_t> active;
    for (size_t i = 0LU; i < num_rank; i++) {
        if ((xsmin[i] - dsrmax <= xslmax) && (xsmax[i] + dsrmax >= xslmin)) {
            active.push_back(i);
        }
    }

    if (opt.verbose) {
        print_x_src_min_max(
            communicator, xslmin, xslmax, xsmin[rank], xsmax[rank], active);
        printx_src_min_max(communicator, xsmin, xsmax);
    }

    auto time = MPI_Wtime();
#define EXSEIS_ONE_WAY_COMM
#ifdef EXSEIS_ONE_WAY_COMM
    auto win = create_coords_win(crd2, opt.ixline);
#else
    std::vector<MPI_Request> reqs;
    {
        auto xsfmin = communicator.gather<fourd_t>(xslmin);
        auto xsfmax = communicator.gather<fourd_t>(xslmax);

        for (size_t i = 0LU; i < num_rank; i++)
            if ((xsmin[rank] - dsrmax <= xsfmax[i])
                && (xsmax[rank] + dsrmax >= xsfmin[i])) {
                std::vector<MPI_Request> rq = sendCrd(i, crd1, opt.ixline);
                reqs.insert(reqs.end(), rq.begin(), rq.end());
            }
    }
    communicator.barrier();
#endif
    // Load balancing would make sense since the workloads are non-symmetric.
    // It might be difficult to do load-balancing though considering workload
    // very much depends on the constraints from both processess

    // Perform the updates of min and minrs using data from other processes.
    // This is the main loop.
    for (size_t i = 0; i < active.size(); i++) {
        double ltime = MPI_Wtime();
        size_t lrank = active[i];
#ifdef EXSEIS_ONE_WAY_COMM
        auto proc = get_coords_win(lrank, szall[lrank], win, opt.ixline);
#else
        auto proc = recvCrd(lrank, szall[lrank], opt.ixline);
#endif
        double wtime = MPI_Wtime() - ltime;
        double sent =
            szall[lrank]
            * (4LU * sizeof(fourd_t) + sizeof(size_t) + 2LU * sizeof(Integer));
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

#ifdef EXSEIS_ONE_WAY_COMM
    for (size_t i = 0; i < win.size(); i++) {
        mpi_err(MPI_Win_free(&win[i]));
    }
#else

    std::vector<MPI_Status> stat(reqs.size());
    mpi_err(MPI_Waitall(reqs.size(), reqs.data(), stat.data()));

#endif
    communicator.barrier();
    cmsg(
        communicator, "Compute phase completed in "
                          + std::to_string(MPI_Wtime() - time) + " seconds");
}

}  // namespace fourdbin
}  // namespace apps
}  // namespace exseis
