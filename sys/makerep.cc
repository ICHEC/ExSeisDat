#include "sglobal.hh"
#include "data/datampiio.hh"
#include "share/segy.hh"
#include "share/smpi.hh"
#include <iostream>
using namespace PIOL;
using namespace Data;

#warning EVENTUALLY WRITE AN OBJECT LAYER TEST


#warning remove
#include <assert.h>
void printmsg(std::string msg, size_t sz, size_t rank, size_t rankn)
{
    std::cout << msg << sz << " " << rank << " " << rankn << std::endl;
}

void distribToDistrib(Piol piol, std::pair<size_t, size_t> old, std::pair<size_t, size_t> newd, std::vector<uchar> * vec)
{
    size_t rank = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();

    std::vector<MPI_Request> msg;
    //if the old offset is less than the new offset
    //Then the local process must give the data over to the
    //-1 lower ranked process
    if (old.first > newd.first) //Cases 2, 3, 8
    {
        size_t incStart = old.first - newd.first;
        vec->resize(vec->size() + incStart);
        std::move_backward(vec->begin(), vec->end() - incStart, vec->end());    //Making space for the move
    }

    if (old.first + old.second < newd.first + newd.second)
       vec->resize(vec->size() + newd.first + newd.second - (old.first + old.second));

    if (old.first < newd.first)
    {
        size_t sz = newd.first - old.first;
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Isend(vec->data(), sz, MPIType<uchar>(), rank-1, 1, MPI_COMM_WORLD, &msg.back());
    }
    else if (old.first > newd.first)
    {
        size_t sz = old.first - newd.first;
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Irecv(vec->data(), sz, MPIType<uchar>(), rank-1, 0, MPI_COMM_WORLD, &msg.back());
    }

    if (old.first + old.second > newd.first + newd.second)
    {
        size_t sz = old.first + old.second - (newd.first + newd.second);
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Isend(vec->data() + vec->size() - sz, sz, MPIType<uchar>(), rank+1, 0, MPI_COMM_WORLD, &msg.back());
    }
    else if (old.first + old.second < newd.first + newd.second)
    {
        size_t sz = (newd.first + newd.second) - (old.first + old.second);
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Irecv(vec->data() + vec->size() - sz, sz, MPIType<uchar>(), rank+1, 1, MPI_COMM_WORLD, &msg.back());
    }

    MPI_Status stat;
    for (size_t i = 0; i < msg.size(); i++)
    {
        assert(msg[i] != MPI_REQUEST_NULL);
        int err = MPI_Wait(&msg[i], &stat);
//TODO: Replace with standard approach to error handling
        if (err != MPI_SUCCESS)
        {
            std::cerr << "Wait " << i << std::endl;
            std::cerr << " MPI Error " << stat.MPI_ERROR << std::endl;
            std::exit(-1);
        }
    }
    if (old.first < newd.first) //Cases 1, 4, 7
    {
        size_t decStart = newd.first - old.first;
        std::move(vec->begin() + decStart, vec->end(), vec->begin());
    }

    vec->resize(newd.second);
}

//Write an arbitrary parallelised block sz to an arbitrary offset with the minimal block contention
//possible between processes.
//writeArb(out, hosz + decSz*i + (fsz - hosz) * (j+1), decSz, &out)
//off is the global offset
std::pair<size_t, size_t> writeArb(Piol piol, Data::Interface * out, size_t off, size_t bsz, std::pair<size_t, size_t> dec, size_t tsz, std::vector<uchar> * vec)
{
    size_t rank = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();
    auto newdec = blockDecomp(tsz, bsz, numRank, rank, off);

    if (numRank != 1)                               //If there is one rank this is pointless
        distribToDistrib(piol, dec, newdec, vec);   //Reorder operations along new boundaries

    size_t flt = ((off + newdec.first) % (2U * 1024U * 1024U));
    assert(!flt || !rank);

    out->write(off + newdec.first, newdec.second, vec->data());

    return newdec;
}

/*! Make a SEGY file by copying the header object and the data payload of an existing file
 *  and writng the header object and then repeatedly writing the payload.
 *  \param[in] piol The piol object
 *  \param[in] iname The SEG-Y input file name
 *  \param[in] oname The SEG-Y output file name
 *  \param[in] repRate The repetition rate
 */
void mpiMakeSEGYCopy(Piol piol, std::string iname, std::string oname, size_t repRate)
{
    size_t rank = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();

    MPIIOOpt opt;
    MPIIO in(piol, iname, opt);

    opt.mode = FileMode::Write;
    MPIIO out(piol, oname, opt);

    csize_t fsz = in.getFileSz();
    csize_t bsz = 2097152LU;
    csize_t hosz = SEGSz::getHOSz();

    size_t memlim = 1335U * bsz;

    size_t step = numRank * memlim;
    for (size_t i = 0; i < fsz; i += step)
    {
        size_t rblock = (i + step < fsz ? step : fsz - i);
        auto dec = blockDecomp(rblock, bsz, numRank, rank, i);

        std::vector<uchar> buf(dec.second);
        in.read(dec.first, dec.second, buf.data());
        piol->isErr();
        out.write(dec.first, dec.second, buf.data());
        piol->isErr();
        if (i == 0)
        {
            if (dec.first == 0)   //If zero, then current process has read the header object
            {
                std::move(buf.begin() + hosz, buf.end(), buf.begin());
                dec.second -= hosz;
                buf.resize(dec.second);
            }
            else
                dec.first -= hosz;
            rblock -= hosz;
        }
        for (size_t j = 1; j < repRate; j++)
            dec = writeArb(piol, &out, hosz + (fsz - hosz) * j + i, bsz, dec, rblock, &buf);
    }
}

void mpiMakeSEGYCopyNaive1(Piol piol, std::string iname, std::string oname, size_t repRate)
{
    size_t rank = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();

    MPIIOOpt opt;
    MPIIO in(piol, iname, opt);

    opt.mode = FileMode::Write;
    MPIIO out(piol, oname, opt);

    csize_t fsz = in.getFileSz();
    csize_t bsz = 2097152LU;
    csize_t hosz = SEGSz::getHOSz();

    size_t memlim = 1335U * bsz;

    size_t step = numRank * memlim;
    for (size_t i = 0; i < fsz; i += step)
    {
        size_t rblock = (i + step < fsz ? step : fsz - i);
        auto dec = blockDecomp(rblock, bsz, numRank, rank);

        std::vector<uchar> buf(dec.second);
        in.read(dec.first, dec.second, buf.data());
        out.write(dec.first, dec.second, buf.data());
        if (i == 0)
            if (dec.first == 0)   //If zero, then current process has read the header object
            {
                std::move(buf.begin() + hosz, buf.begin() + dec.second, buf.begin());
                dec.second -= hosz;
                buf.resize(dec.second);
            }
            else
                dec.first -= hosz;

        for (size_t j = 1; j < repRate; j++)
            out.write(hosz + (fsz - hosz) * j + i + dec.first, dec.second, buf.data());
    }
}

void mpiMakeSEGYCopyNaive2(Piol piol, std::string iname, std::string oname, size_t repRate)
{
    size_t rank = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();

    MPIIOOpt opt;
    MPIIO in(piol, iname, opt);

    opt.mode = FileMode::Write;
    MPIIO out(piol, oname, opt);

    csize_t fsz = in.getFileSz();
    csize_t bsz = 2097152LU;
    csize_t hosz = SEGSz::getHOSz();

    size_t memlim = 1335U * bsz;

    size_t step = numRank * memlim;
    for (size_t i = 0; i < fsz; i += step)
    {
        size_t rblock = (i + step < fsz ? step : fsz - i);
        auto dec = decompose(rblock, numRank, rank);

        std::vector<uchar> buf(dec.second);
        in.read(dec.first, dec.second, buf.data());
        out.write(dec.first, dec.second, buf.data());
        if (i == 0)
            if (dec.first == 0)   //If zero, then current process has read the header object
            {
                std::move(buf.begin() + hosz, buf.begin() + dec.second, buf.begin());
                dec.second -= hosz;
                buf.resize(dec.second);
            }
            else
                dec.first -= hosz;

        for (size_t j = 1; j < repRate; j++)
            out.write(hosz + (fsz - hosz) * j + i + dec.first, dec.second, buf.data());
    }
}

