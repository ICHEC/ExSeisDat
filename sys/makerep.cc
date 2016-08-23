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

void distribToDistrib(Piol piol, std::pair<size_t, size_t> old, std::pair<size_t, size_t> newd, std::vector<uchar> * vec)
{
    size_t rank = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();

    std::vector<MPI_Request> msg;
    //if the old offset is less than the new offset
    //Then the local process must give the data over to the
    //-1 lower ranked process
    std::vector<size_t> lvec, rvec;

///////////// SENDS ////////////////////
    std::cout << "First " << old.first << " " << newd.first << std::endl;
    std::cout << "Secnd " << old.second << " " << newd.second << std::endl;

    if (old.first < newd.first)
    {
        if (rank == 0)
            std::cerr << "Something has gone horribly wrong\n";
        size_t sz = newd.first - old.first;

        lvec.resize(sz);

        std::move(vec->begin(), vec->begin() + sz, lvec.begin());
        std::move_backward(vec->begin() + sz, vec->end(), vec->begin());

        msg.push_back(MPI_REQUEST_NULL);
        MPI_Isend(lvec.data(), sz, MPIType<uchar>(),
                    rank, 0, MPI_COMM_WORLD, &msg.back());
    }

    if (old.first > newd.first)
    {
        if (rank == 0)
            std::cerr << "Something has gone horribly wrong\n";
        size_t sz = old.first - newd.first;

        if (vec->size() < newd.second)
            vec->resize(newd.second);
        std::move(vec->begin(), vec->end(), vec->begin() + sz);

        MPI_Irecv(vec->data(), sz, MPIType<uchar>(),
                    rank, 0, MPI_COMM_WORLD, &msg.back());

    }
 
    if (old.first + old.second > newd.first + newd.second)
    {
        if (rank == numRank -1)
            std::cerr << "Something has gone horribly wrong\n";
        size_t sz = old.first + old.second - (newd.first + newd.second);

        rvec.resize(sz);

        std::move(vec->end() - sz, vec->end(), rvec.begin());

        msg.push_back(MPI_REQUEST_NULL);
        MPI_Isend(rvec.data(), sz, MPIType<uchar>(),
                    rank, 1, MPI_COMM_WORLD, &msg.back());
    }

    if (old.first + old.second < newd.first + newd.second)
    {
        if (rank == numRank -1)
            std::cerr << "Something has gone horribly wrong\n";
        size_t sz = old.first + old.second - (newd.first + newd.second);
        if (vec->size() < newd.second)
            vec->resize(newd.second);

        msg.push_back(MPI_REQUEST_NULL);
        MPI_Irecv(vec->data() + newd.second - sz, sz, MPIType<uchar>(),
                    rank, 1, MPI_COMM_WORLD, &msg.back());
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
}

//Write an arbitrary parallelised block sz to an arbitrary offset with the minimal block contention
//possible between processes.
//writeArb(out, hosz + pieceSz*i + (fsz - hosz) * (j+1), pieceSz, &out)
//off is the global offset
std::pair<size_t, size_t> writeArb(Piol piol, Data::Interface * out, size_t bsz, size_t off, std::pair<size_t, size_t> dec, std::vector<uchar> * vec)
{
    size_t loff = dec.first; //local offset
    size_t sz = dec.second; //local sz

    size_t rank = piol->comm->getRank();
    size_t numRank = piol->comm->getNumRank();

    ///////////////Calculate a new decomposition////////
    size_t bcnt = sz / bsz;
    size_t rstart = off % bsz;
    size_t rend = (off + sz) % bsz;
    bcnt += (rstart != 0) + (rend != 0);

    auto newdec = decompose(bcnt, numRank, rank);

    newdec.first *= bsz;
    newdec.second *= bsz;

    std::cout << rstart << " " << rend << std::endl;
    if (!rank)
        newdec.second -= rstart;
    else
        newdec.first -= rstart;
    if (rank == numRank-1)
        newdec.second -= rend;

    ///////////////////////////////////////////////////
    distribToDistrib(piol, dec, newdec, vec);    //Reorder operations along new boundaries

    out->write(off, newdec.second, vec->data());
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

    opt.mode = MPI_MODE_CREATE | MPI_MODE_WRONLY;
    MPIIO out(piol, oname, opt);

    csize_t fsz = in.getFileSz();
    csize_t bsz = 2097152LU;
    csize_t mem = 1200LU*bsz;
    csize_t hosz = SEGSz::getHOSz();

    auto piece = blockDecomp(fsz, bsz, numRank, rank);
    for (size_t i = 0; i < piol->comm->getNumRank(); i++)
    {
        if (i == piol->comm->getRank())
           std::cout << i << " f " << piece.first << " s " << piece.second << std::endl;
        piol->comm->barrier();
    }

    std::vector<uchar> buf(piece.second);
    in.read(piece.first, piece.second, buf.data());
    out.write(piece.first, piece.second, buf.data());

    if (piece.first == 0)   //If zero, then current process has read the header object
    {
        piece.second -= hosz;
        std::move(buf.begin() + hosz, buf.begin() + piece.second, buf.begin());
    }
    else
        piece.first -= hosz;

    size_t j = 1;
    writeArb(piol, &out, bsz, hosz + (fsz - hosz) * j, piece, &buf);
}

