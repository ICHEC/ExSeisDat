#include <assert.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "cppfileapi.hh"
#include "sglobal.hh"
#include "share/mpi.hh"
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


//This is a lighter check for test purposes. Just the absolute values
inline geom_t dsrLight(const geom_t * prm1, const geom_t * prm2)
{
    return std::abs(prm1[0] - prm2[0]) +
           std::abs(prm1[1] - prm2[1]) +
           std::abs(prm1[2] - prm2[2]) +
           std::abs(prm1[3] - prm2[3]);
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
inline geom_t dsr(const geom_t * prm1, const geom_t * prm2)
{
    //Calculate ds = sqrt((s_x^1 - s_x^2)^2 + (s_y^1 - s_y^2)^2)
    geom_t fds = std::sqrt(pow((prm1[0] - prm2[0]), 2) + pow((prm1[1] - prm2[1]), 2));

    //Calculate dr = sqrt((r_x^1 - r_x^2)^2 + (r_y^1 - r_y^2)^2)
    geom_t fdr = std::sqrt(pow((prm1[2] - prm2[2]), 2) + pow((prm1[3] - prm2[3]), 2));

    //Reverse-Boat cases. 
    //Calculate sqrt((s_x^1 - r_x^2)^2 + (s_y^1 - r_y^2)^2)
    geom_t rds = std::sqrt(pow((prm1[0] - prm2[2]), 2) + pow((prm1[1] - prm2[3]), 2));
    //Calculate sqrt((r_x^1 - s_x^2)^2 + (r_y^1 - s_y^2)^2)
    //TODO: This might now be avoidable with a ternary
    geom_t rdr = std::sqrt(pow((prm1[2] - prm2[0]), 2) + pow((prm1[3] - prm2[1]), 2));

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

/*! This function extracts the relevant parameters from the file and inserts them into a vector (coords)
 *  \param[in] piol The piol handle, used for MPI collectives.
 *  \param[in] file The input file to access parameters from.
 *  \param[in] offset The offset for the local process to access from
 *  \param[in] coords The vector for storing the parameters. Number of parameters is coords.size()/4
 */
void getCoords(ExSeisPIOL * piol, File::Interface * file, size_t offset, vec<geom_t> & coords)
{
    //This makes a rule about what data we will access. In this particular case it's xsrc, ysrc, xrcv, yrcv.
    //Unfortunately shared pointers make things ugly in C++.
    //without shared pointers it would be Rule rule = { Meta::xSrc, Meta::ySrc, Meta::xRcv, Meta::yRcv };
    auto rule = std::make_shared<Rule>(std::initializer_list<Meta>{Meta::xSrc, Meta::ySrc, Meta::xRcv, Meta::yRcv});
    size_t lnt = coords.size()/4U;

    /*These two lines are for some basic memory limitation calculations. In future versions of the PIOL this will be
      handled internally and in a more accurate way. User Story S-01490. The for loop a few lines below reads the trace
      parameters in batches because of this memory limit.*/
    size_t memlim = 2U*1024U*1024U*1024U - coords.size() * sizeof(geom_t);
    size_t max = memlim / (rule->paramMem() + SEGSz::getMDSz());

    //Collective I/O requries an equal number of MPI-IO calls on every process in exactly the same sequence as each other.
    //If not, the code will deadlock. Communication is done to ensure we balance out the correct number of redundant calls
    size_t biggest = piol->comm->max(max);
    size_t extra = biggest/max - lnt/max + (biggest % max > 0) - (lnt % max > 0);

    for (size_t i = 0; i < lnt; i += max)
    {
        size_t rblock = (i + max < lnt ? max : lnt - i);
        File::Param prm(rule, rblock);
        file->readParam(offset+i, rblock, &prm);
        for (size_t j = 0; j < rblock; j++)
        {
            coords[4U*(i+j)+0] = File::getPrm<geom_t>(i+j, Meta::xSrc, &prm);
            coords[4U*(i+j)+1] = File::getPrm<geom_t>(i+j, Meta::ySrc, &prm);
            coords[4U*(i+j)+2] = File::getPrm<geom_t>(i+j, Meta::xRcv, &prm);
            coords[4U*(i+j)+3] = File::getPrm<geom_t>(i+j, Meta::yRcv, &prm);
        }
    }
    //Any extra readParam calls the particular process needs
    for (size_t i = 0; i < extra; i++)
        file->readParam(0U, size_t(0), nullptr);
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

    //TODO:: Implement reduction on an accelerator
    for (size_t j = (Init ? 1U : 0U); j < szall[orank]; j++)    //Loop through every file2 trace
        for (size_t i = 0; i < sz; i++)                         //Loop through every file1 trace
        {
            geom_t dval = dsr(&local[4U*i], &other[4U*j]);      //Get dsdr
            min[i] = (dval < minrs[i] ? offset + j : min[i]);   //Update min if applicable
            minrs[i] = std::min(dval, minrs[i]);                //Update minrs if applicable
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
        src.readTrace(0, nullptr, nullptr, const_cast<File::Param *>(File::PARAM_NULL));
        dst.writeTrace(0, size_t(0), nullptr, File::PARAM_NULL);
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

/*    for (size_t i = 0; i < piol->comm->getNumRank(); i++)
    {
        piol->comm->barrier();
        if (i == piol->comm->getRank())
            std::cout << i << " " << biggest << " " << lnt << "  " << max << " " << extra << std::endl;
        piol->comm->barrier();
    }*/

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
        src.readTrace(0, nullptr, nullptr, const_cast<File::Param *>(File::PARAM_NULL));
        dst.writeTrace(0, size_t(0), nullptr, File::PARAM_NULL);
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

    vec<geom_t> coords1;
    vec<geom_t> coords2;
    size_t sz[2];

    cmsg(piol, "Parameter-read phase");
    //Perform the decomposition and read the coordinates of interest.
    {
        auto dec1 = decompose(file1.readNt(), numRank, rank);
        auto dec2 = decompose(file2.readNt(), numRank, rank);

        sz[0] = dec1.second;
        coords1.resize(4U*sz[0]);
        getCoords(ppiol, file1, dec1.first, coords1);

        sz[1] = dec2.second;
        coords2.resize(4U*sz[1]);
        getCoords(ppiol, file2, dec2.first, coords2);
    }

    vec<size_t> min(sz[0]);
    vec<geom_t> minrs(sz[0]);

    auto szall = ppiol->comm->gather(vec<size_t>{sz[1]});

    cmsg(piol, "Compute phase");
    cmsg(piol, "Round 1  of " + std::to_string(numRank));
    //Perform a local update of min and minrs
    update<true>(szall, coords1, rank, coords2, min, minrs);

    //Perform the updates of min and minrs using data from other processes.
    for (size_t i = 1U; i < numRank; i ++)
    {
        cmsg(piol, "Round " + std::to_string(i + 1) +  " of " + std::to_string(numRank));
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

    //free up some memory
    coords1.resize(0);
    coords2.resize(0);
    coords1.shrink_to_fit();
    coords2.shrink_to_fit();

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

    //Open and write out file1 --> file3
    File::Direct file3(piol, name3, FileMode::Write);
    select(piol, rule, file3, file1, list1, minrs);

    //Open and write out file2 --> file4
    //This case is more complicated because the list is unordered and there  can be duplicate entries
    //in the list.
    File::Direct file4(piol, name4, FileMode::Write);
    selectDupe(piol, rule, file4, file2, list2, minrs);

    return 0;
}
