#include <assert.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "cppfileapi.hh"
#include "fileops.hh"   //For sort
#include "sglobal.hh"
#include "share/mpi.hh"
#define ALIGN 32U
using namespace PIOL;
using File::Tr;
using File::Rule;

template <class T>
using cvec = const std::vector<T>;

template <class T>
using vec = std::vector<T>;

void cmsg(ExSeisPIOL * piol, std::string msg)
{
    piol->comm->barrier();
    if (!piol->comm->getRank())
        std::cout << msg << std::endl;
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
inline geom_t dsr(geom_t xs1, geom_t ys1, geom_t xr1, geom_t yr1,
           geom_t xs2, geom_t ys2, geom_t xr2, geom_t yr2)
{
    geom_t fds = std::hypot(xs1 - xs2, ys1 - ys2);
    geom_t fdr = std::hypot(xr1 - xr2, yr1 - yr2);

    //Reverse-Boat cases.
    geom_t rds = std::hypot(xs1 - xr2, ys1 - yr2);
    geom_t rdr = std::hypot(xr1 - xs2, yr1 - ys2);

    geom_t ds = std::min(fds, rds);
    geom_t dr = std::min(fdr, rdr);

    //The sum of ds and dr will be minimised.
    //ds=0, dr=10 will be preferred over
    //ds=4.55 dr=5.5
    return ds + dr;

    //This gives preference to a value that minimises
    //both ds and dr to some extent.
    //ds=4.55 dr=5.5 will be preferred over
    //ds=0, dr=10
//    return ds + dr + std::abs(ds-dr);
}

/*! This structure is for holding ALIGN aligned memory containing the coordinates.
 */
struct Coords
{
    size_t sz;
    geom_t * xSrc;
    geom_t * ySrc;
    geom_t * xRcv;
    geom_t * yRcv;
    size_t * tn;
    const size_t align = ALIGN;
    size_t allocSz;
    Coords(size_t sz_) : sz(sz_)
    {
        allocSz = ((sz + align) / align) * align;

        //posix_memalign() guarantees the memory allocated is alligned according to the alignment
        //value
        posix_memalign(reinterpret_cast<void **>(&xSrc), align, allocSz * sizeof(geom_t));
        posix_memalign(reinterpret_cast<void **>(&ySrc), align, allocSz * sizeof(geom_t));
        posix_memalign(reinterpret_cast<void **>(&xRcv), align, allocSz * sizeof(geom_t));
        posix_memalign(reinterpret_cast<void **>(&yRcv), align, allocSz * sizeof(geom_t));
        posix_memalign(reinterpret_cast<void **>(&tn), align, allocSz * sizeof(size_t));

        for (size_t i = 0; i < allocSz; i++)
            xSrc[i] = ySrc[i] = xRcv[i] = yRcv[i] = std::numeric_limits<float>::max();
    }

    ~Coords(void)
    {
        if (xSrc)
            free(xSrc);
        if (ySrc)
            free(ySrc);
        if (xRcv)
            free(xRcv);
        if (yRcv)
            free(yRcv);
    }
};

/*! This function extracts the relevant parameters from the file and inserts them into a vector (coords)
 *  \param[in] piol The piol handle, used for MPI collectives.
 *  \param[in] file The input file to access parameters from.
 *  \param[in] offset The offset for the local process to access from
 *  \param[in] coords The vector for storing the parameters. Number of parameters is coords.size()/4
 */
void getCoords(ExSeisPIOL * piol, File::Interface * file, size_t offset, Coords * coords)
{
    auto rule = std::make_shared<Rule>(std::initializer_list<Meta>{Meta::ltn, Meta::xSrc});
    size_t lnt = coords->sz;

    /*These two lines are for some basic memory limitation calculations. In future versions of the PIOL this will be
      handled internally and in a more accurate way. User Story S-01490. The for loop a few lines below reads the trace
      parameters in batches because of this memory limit.*/
    size_t memlim = 2U*1024U*1024U*1024U - 4U * coords->sz * sizeof(geom_t);
    size_t max = memlim / (rule->paramMem() + SEGSz::getMDSz());

    //Collective I/O requries an equal number of MPI-IO calls on every process in exactly the same sequence as each other.
    //If not, the code will deadlock. Communication is done to ensure we balance out the correct number of redundant calls
    size_t biggest = piol->comm->max(max);
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

    File::Param prm(rule, lnt);
    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        file->readParam(offset+i, rblock, &prm, i);

        for (size_t j = 0; j < rblock; j++)
            setPrm(i + j, Meta::ltn, offset + i + j, &prm);
    }

    //Any extra readParam calls the particular process needs
    for (size_t i = 0; i < extra; i++)
        file->readParam(0U, size_t(0), nullptr, 0U);

//TODO: Replace with set layer call?

    auto trlist = File::sort(piol, offset, &prm, [] (const File::Param & e1, const File::Param & e2) -> bool
            {
                return (File::getPrm<geom_t>(0U, Meta::xSrc, &e1) < File::getPrm<geom_t>(0U, Meta::xSrc, &e2) ? true :
                        File::getPrm<llint>(0U, Meta::ltn, &e1) < File::getPrm<llint>(0U, Meta::ltn, &e2));
            });

    //This makes a rule about what data we will access. In this particular case it's xsrc, ysrc, xrcv, yrcv.
    //Unfortunately shared pointers make things ugly in C++.
    //without shared pointers it would be Rule rule = { Meta::xSrc, Meta::ySrc, Meta::xRcv, Meta::yRcv };
    auto crule = std::make_shared<Rule>(std::initializer_list<Meta>{Meta::xSrc, Meta::ySrc, Meta::xRcv, Meta::yRcv});

    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        File::Param prm(crule, rblock);
        file->readParam(rblock, trlist.data() + i, &prm);
        for (size_t j = 0; j < rblock; j++)
        {
            coords->xSrc[i+j] = File::getPrm<geom_t>(i+j, Meta::xSrc, &prm);
            coords->ySrc[i+j] = File::getPrm<geom_t>(i+j, Meta::ySrc, &prm);
            coords->xRcv[i+j] = File::getPrm<geom_t>(i+j, Meta::xRcv, &prm);
            coords->yRcv[i+j] = File::getPrm<geom_t>(i+j, Meta::yRcv, &prm);
            coords->tn[i+j] = trlist[i+j];
        }
    }
    //Any extra readParam calls the particular process needs
    for (size_t i = 0; i < extra; i++)
        file->readParam(0U, nullptr, nullptr, 0);
}

/*! Perform a minimisation check with the current two vectors of parameters.
 *  \tparam Init If true, perform the initialisation sequence.
 *  \param[in] szall A vector containing the amount of data each process has from the second input file.
 *  \param[in] local A vector containing the process's parameter data from the first input file. This data
 *             is never sent to any other process.
 *  \param[in] other A vector containing the parameter data from another process.
 *  \param[in,out] min A vector containing the trace number of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 *  \param[in,out] minrs A vector containing the dsdr value of the trace that minimises the dsdr criteria.
 *                 This vector is updated by the loop.
 */
template <bool Init>
void update(size_t offset, Coords * local, Coords * other, vec<size_t> & min, vec<geom_t> & minrs)
{
    size_t sz = local->sz;
    //For the vectorisation
    geom_t * lxS = local->xSrc;
    geom_t * lyS = local->ySrc;
    geom_t * lxR = local->xRcv;
    geom_t * lyR = local->yRcv;

    geom_t * rxS = other->xSrc;
    geom_t * ryS = other->ySrc;
    geom_t * rxR = other->xRcv;
    geom_t * ryR = other->yRcv;

    size_t * tn = other->tn;

    if (Init)
    {
        #pragma omp simd aligned(lxS:ALIGN) aligned(lyS:ALIGN) aligned(lxR:ALIGN) aligned(lyR:ALIGN) \
                         aligned(rxS:ALIGN) aligned(ryS:ALIGN) aligned(rxR:ALIGN) aligned(ryR:ALIGN) \
                         aligned(tn:ALIGN)
        for (size_t i = 0; i < sz; i++)
            minrs[i] = dsr(lxS[i], lyS[i], lxR[i], lyR[i],
                           rxS[0], ryS[0], rxR[0], ryR[0]);
        std::copy(tn, tn + sz, min.begin());
    }

    for (size_t i = 0; i < sz; i++)                         //Loop through every file1 trace
    {
        size_t lmin = min[i];   //temporary variables are improving optimisation potential
        geom_t lminrs = minrs[i];
        #pragma omp simd aligned(rxS:ALIGN) aligned(ryS:ALIGN) aligned(rxR:ALIGN) aligned(ryR:ALIGN) \
                         aligned(tn:ALIGN)
        for (size_t j = 0U; j < other->allocSz; j++)        //loop through a multiple of the alignment
            {
                geom_t dval = dsr(lxS[i], lyS[i], lxR[i], lyR[i],
                                  rxS[j], ryS[j], rxR[j], ryR[j]);
                lmin = (dval < lminrs ? tn[j] : lmin);    //Update min if applicable
                lminrs = std::min(dval, lminrs);            //Update minrs if applicable
            }
        min[i] = lmin;
        minrs[i] = lminrs;
    }
}

//This trick is discussed here:
//http://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
vec<size_t> getSortIndex(size_t sz, size_t * list)
{
    vec<size_t> index(sz);
    std::iota(index.begin(), index.end(), 0);
    std::sort(index.begin(), index.end(), [list] (size_t s1, size_t s2) { return list[s1] < list[s2]; });
    return index;
}

//TODO: Have a mechanism to change from one Param representation to another?
// This is an output related function and doesn't change the core algorithm.
void selectDupe(ExSeisPIOL * piol, std::shared_ptr<Rule> rule, File::Direct & dst, File::Direct & src, vec<size_t> & list, vec<geom_t> & minrs)
{
    size_t ns = src.readNs();
    size_t lnt = list.size();
    size_t offset = 0;
    size_t biggest = 0;
    size_t sz = 0;
    {
        auto nts = piol->comm->gather(vec<size_t>{lnt});
        for (size_t i = 0; i < nts.size(); i++)
        {
            if (i == piol->comm->getRank())
                offset = sz;
            sz += nts[i];
            biggest = std::max(biggest, nts[i]);
        }
    }

    size_t memused = lnt * (sizeof(size_t) + sizeof(geom_t));
    size_t memlim = 2U*1024U*1024U*1024U;
    assert(memlim > memused);
    size_t max = (memlim - memused) / (4U*SEGSz::getDOSz(ns) + 4U*rule->extent());
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

    dst.writeText("ExSeisDat 4d-bin file.\n");
    dst.writeNt(sz);
    dst.writeInc(src.readInc());
    dst.writeNs(ns);

    File::Param prm(rule, std::min(lnt, max));
    vec<trace_t> trc(ns * std::min(lnt, max));

    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);

        auto idx = getSortIndex(rblock, &list[i]);
        std::vector<size_t> nodups;
        for (size_t j = 0; j < rblock; j++)
            if (!j || list[idx[j-1]] != list[idx[j]])
                nodups.push_back(list[idx[j]]);

        File::Param sprm(rule, nodups.size());
        vec<trace_t> strc(ns * nodups.size());
        src.readTrace(nodups.size(), nodups.data(), strc.data(), &sprm);

        size_t n = 0;
        for (size_t j = 0; j < rblock; j++)
        {
            if (!j || list[idx[j-1]] != list[idx[j]])
                n = j;
            cpyPrm(n, &sprm, idx[n], &prm);

            setPrm(j, Meta::dsdr, minrs[j], &prm);
            for (size_t k = 0; k < ns; k++)
                trc[j*ns + k] = strc[idx[n]*ns + k];
        }

        dst.writeTrace(offset+i, rblock, trc.data(), &prm);
    }

    for (size_t i = 0; i < extra; i++)
    {
        src.readTrace(0, nullptr, nullptr, nullptr);
        dst.writeTrace(0, size_t(0), nullptr, nullptr);
    }
}

template <bool AllProc = true>
void recordTime(ExSeisPIOL * piol, std::string msg, double startTime)
{
    auto time = MPI_Wtime();

    if (AllProc)
    {
        auto pTimeTot = piol->comm->gather(vec<float>{static_cast<float>(time - startTime)});
        if (!piol->comm->getRank())
            for (size_t i = 0; i < pTimeTot.size(); i++)
                std::cout << msg << " time: " << i << " " << pTimeTot[i] << std::endl;
        piol->comm->barrier();
    }
    else
    {
        auto pTimeTot = (time - startTime);
        if (!piol->comm->getRank())
            std::cout << msg << " time: " << pTimeTot << std::endl;
    }
}

// This is an output related function and doesn't change the core algorithm.
void select(ExSeisPIOL * piol, std::shared_ptr<Rule> rule, File::Direct & dst, File::Direct & src, vec<size_t> & list, vec<geom_t> & minrs)
{
    const size_t ns = src.readNs();
    const size_t lnt = list.size();
    dst.writeText("ExSeisDat 4d-bin file.\n");
    size_t offset = 0;
    size_t biggest = 0;
    size_t sz = 0;
    {
        auto offsets = piol->comm->gather(vec<size_t>{lnt});
        for (size_t i = 0; i < offsets.size(); i++)
        {
            if (i == piol->comm->getRank())
                offset = sz;
            sz += offsets[i];
            biggest = std::max(biggest, offsets[i]);
        }
    }
    size_t memused = lnt * (sizeof(size_t) + sizeof(geom_t));
    size_t memlim = 2U*1024U*1024U*1024U;
    assert(memlim > memused);

    size_t max = (memlim - memused) / (4U*SEGSz::getDOSz(ns) + 4U*rule->extent());
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

    dst.writeNt(sz);
    dst.writeInc(src.readInc());
    dst.writeNs(src.readNs());

    File::Param prm(rule, std::min(lnt, max));
    vec<trace_t> trc(src.readNs() * std::min(lnt, max));

    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);

        src.readTrace(rblock, &list[i], trc.data(), &prm);
        for (size_t j = 0; j < rblock; j++)
            setPrm(j, Meta::dsdr, minrs[j], &prm);
        dst.writeTrace(offset+i, rblock, trc.data(), &prm);
    }

    for (size_t i = 0; i < extra; i++)
    {
        src.readTrace(0, nullptr, nullptr, nullptr);
        dst.writeTrace(0, size_t(0), nullptr, nullptr);
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
        cmsg(piol, "getCoords1");
        coords1 = std::make_unique<Coords>(sz[0]);
        getCoords(ppiol, file1, dec1.first, coords1.get());

        sz[1] = dec2.second;
        cmsg(piol, "getCoords2");
        coords2 = std::make_unique<Coords>(sz[1]);
        getCoords(ppiol, file2, dec2.first, coords2.get());
    }
    cmsg(piol, "post-Read");

    vec<size_t> min(sz[0]);
    vec<geom_t> minrs(sz[0]);

    auto szall = ppiol->comm->gather(vec<size_t>{sz[1]});
    std::vector<size_t> offset(szall.size());
    for (size_t i = 1; i < offset.size(); i++)
        offset[i] = offset[i-1] + szall[i];

    recordTime(piol, "parameter", startTime);
    cmsg(piol, "Compute phase");

#ifdef RANDOM_WRITE
    srand(1337);
    size_t nt = file2.readNt();
    for (size_t i = 0; i < sz[0]; i++)
        min[i] = ((llint(rand()) << 8) & llint(rand())) % nt;
#else
    startTime = MPI_Wtime();
    cmsg(piol, "Round 0 of " + std::to_string(numRank-1));
    //Perform a local update of min and minrs


#warning TEMPORARY!
#warning TEMPORARY!
#warning TEMPORARY!
#warning TEMPORARY!
#warning TEMPORARY!
//    update<true>(offset[rank], coords1.get(), coords2.get(), min, minrs);
    recordTime(piol, "update 0", startTime);

    auto xsmin = ppiol->comm->gather(vec<geom_t>{coords2->xSrc[0U]});
    auto xsmax = ppiol->comm->gather(vec<geom_t>{coords2->xSrc[coords2->sz-1U]});

    //Perform the updates of min and minrs using data from other processes.
    for (size_t i = 1U; i < numRank; i ++)
    {
        cmsg(piol, "Round " + std::to_string(i) +  " of " + std::to_string(numRank));
        size_t lrank = (rank + numRank - i) % numRank;  //The rank of the left process
        size_t rrank = (rank + i) % numRank;            //The rank of the right process

        //Could the process on the left have data I want?
        size_t IRcv = (xsmax[lrank] + dsrmax > xsmin[rank] && xsmin[lrank] + dsrmax < xsmax[rank]);
        size_t ISnd = (xsmax[rank] + dsrmax > xsmin[rrank] && xsmin[rank] + dsrmax < xsmax[rrank]);

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
        {
            for (size_t j = 0; j < rmsg.size(); j++)
                assert(MPI_Wait(&rmsg[j], &stat) == MPI_SUCCESS);         //TODO: Replace with standard approach to error handling
        }

        recordTime(piol, "snd/rcv " + std::to_string(i), startTime);
        startTime = MPI_Wtime();

#warning TEMPORARY!
#warning TEMPORARY!
#warning TEMPORARY!
#warning TEMPORARY!
#warning TEMPORARY!
//        if (IRcv)
//            update<false>(offset[lrank], coords1.get(), proc.get(), min, minrs);

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
