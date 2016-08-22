#include "sglobal.hh"
#include "data/datampiio.hh"
#include "share/segy.hh"
#include <iostream>
using namespace PIOL;
using namespace Data;

#warning EVENTUALLY WRITE AN OBJECT LAYER TEST


std::pair<size_t, size_t> sendBorder(Piol piol, std::vector<uchar> * vec, size_t offset, size_t sz, size_t bsz, size_t fsz)
{
    std::vector<MPI_Request> msg;

    size_t lq = offset / bsz;
    size_t lr = (offset % bsz != 0);
    size_t hq = (offset + sz)/ bsz;
    size_t hr = ((offset + sz) % bsz != 0);
    size_t low = (lq + lr) * bsz;
    size_t high = std::min((hq + hr) * bsz, fsz);

    size_t size = high - low;
    size_t sndLow = low - offset;
    size_t rcvHigh = high - offset-sz;

    if (piol->comm->getRank() && low-offset)
    {
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Isend(vec.data(), sendLow, MPIType<size_t>(),
                  piol->comm->getRank()-1, 0, MPI_COMM_WORLD, &msg[0]);
    }
    std::vector<uchar> rcv(rcvHigh);
    if (piol->comm->getRank() != piol->comm->getNumRank()-1 && high-offset-sz)
    {
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Irecv(tSort1.data(), regionSz * sizeof(std::pair<size_t, T>), MPI_CHAR,
                  comm.getRank()-1, 1, MPI_COMM_WORLD, &msg.back());
    }

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
    MPIIOOpt opt;
    MPIIO in(piol, iname, opt);

    opt.mode = MPI_MODE_CREATE | MPI_MODE_WRONLY;
    MPIIO out(piol, oname, opt);

    size_t fsz = in.getFileSz();
//    out.setFileSz(fsz*10U - 3600U*9U);

    if (!piol->comm->getRank())
    {
        std::vector<uchar> header(SEGSz::getHOSz());
        in.read(0U, header.size(), header.data());
        out.write(0U, header.size(), header.data());
    }

    //auto piece = blockDecomp(fsz-SEGSz::getHOSz(), 2097152U, piol->comm->getNumRank(), piol->comm->getRank());
//Write first set of blocks
    auto piece = blockDecomp(fsz, 2097152U, piol->comm->getNumRank(), piol->comm->getRank());
    for (size_t i = 0; i < piol->comm->getNumRank(); i++)
    {
        if (i == piol->comm->getRank())
           std::cout << i << " " << piece.first << " " << piece.second << std::endl;
        piol->comm->barrier();
    }

    size_t pieceSz = std::min(2LU*1024LU*1024LU*1024LU, piece.second);
    std::vector<uchar> buf(pieceSz);
    size_t q = piece.second / pieceSz;
    size_t r = piece.second % pieceSz;
    for (size_t i = 0; i < q; i++)
    {
        size_t offset = SEGSz::getHOSz() + piece.first + pieceSz*i;
        in.read(offset, buf.size(), buf.data());
        out.write(offset, buf.size(), buf.data());
    }
    size_t offset = SEGSz::getHOSz() + piece.first + pieceSz*q;
    in.read(offset, r, buf.data());
    out.write(offset + (fsz-SEGSz::getHOSz()), r, buf.data());

//// Exchange on borders
    if (repRate > 0)
    {
        //Send to neighbours so there is blocked workloads
        //then repeatedly write.


        size_t q = piece.second / pieceSz;
        size_t r = piece.second % pieceSz;
        for (size_t i = 0; i < q; i++)
        {
            size_t offset = SEGSz::getHOSz() + piece.first + pieceSz*i;
            in.read(offset, buf.size(), buf.data());

            for (size_t j = 0; j < repRate; j++)
                out.write(offset + (fsz-SEGSz::getHOSz()) * j, buf.size(), buf.data());
        }
        size_t offset = SEGSz::getHOSz() + piece.first + pieceSz*q;
        in.read(offset, r, buf.data());
        for (size_t j = 0; j < repRate; j++)
            out.write(offset + (fsz-SEGSz::getHOSz()) * j, r, buf.data());

    }
}

