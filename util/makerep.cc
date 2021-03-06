#include "sglobal.hh"

#include "ExSeisDat/PIOL/DataMPIIO.hh"
#include "ExSeisDat/PIOL/ExSeis.hh"
#include "ExSeisDat/PIOL/segy_utils.hh"
#include "ExSeisDat/utils/mpi/MPI_type.hh"

#include <assert.h>
#include <iostream>
#include <unistd.h>
#include <unordered_map>

using namespace exseis::utils;
using namespace exseis::PIOL;

void printmsg(std::string msg, size_t sz, size_t rank, size_t rankn)
{
    std::cout << msg << sz << " " << rank << " " << rankn << std::endl;
}

void smallCopy(
  const ExSeis& piol, DataInterface* in, DataInterface* out, size_t repRate)
{
    const size_t rank = piol.comm->getRank();

    const size_t fsz  = in->getFileSz();
    const size_t hosz = SEGY_utils::getHOSz();
    size_t wsz        = (rank == 0 ? fsz : 0);
    std::vector<unsigned char> buf(wsz);

    in->read(0, wsz, buf.data());
    piol.isErr();

    out->write(0, wsz, buf.data());
    piol.isErr();

    if (rank == 0) {
        std::copy(buf.begin() + hosz, buf.end(), buf.begin());
    }

    for (size_t j = 1; j < repRate; j++) {
        out->write(
          hosz + (fsz - hosz) * j, (rank == 0 ? fsz - hosz : 0), buf.data());
        piol.isErr();
    }
}

void distribToDistrib(
  size_t rank,
  exseis::utils::Contiguous_decomposition old,
  exseis::utils::Contiguous_decomposition newd,
  std::vector<unsigned char>* vec)
{
    std::vector<MPI_Request> msg;

    // if the old offset is less than the new offset
    // Then the local process must give the data over to the
    //-1 lower ranked process

    // Cases 2, 3, 8
    if (old.global_offset > newd.global_offset) {
        size_t incStart = old.global_offset - newd.global_offset;
        vec->resize(vec->size() + incStart);
        // Making space for the move
        std::move_backward(vec->begin(), vec->end() - incStart, vec->end());
    }

    if (
      (old.global_offset + old.local_size)
      < (newd.global_offset + newd.local_size)) {

        vec->resize(
          vec->size() + newd.global_offset + newd.local_size
          - (old.global_offset + old.local_size));
    }

    if (old.global_offset < newd.global_offset) {
        size_t sz = newd.global_offset - old.global_offset;
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Isend(
          vec->data(), sz, exseis::utils::MPI_type<unsigned char>(), rank - 1,
          1, MPI_COMM_WORLD, &msg.back());
    }
    else if (old.global_offset > newd.global_offset) {
        size_t sz = old.global_offset - newd.global_offset;
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Irecv(
          vec->data(), sz, exseis::utils::MPI_type<unsigned char>(), rank - 1,
          0, MPI_COMM_WORLD, &msg.back());
    }

    if (
      old.global_offset + old.local_size
      > newd.global_offset + newd.local_size) {
        size_t sz = old.global_offset + old.local_size
                    - (newd.global_offset + newd.local_size);
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Isend(
          vec->data() + vec->size() - sz, sz,
          exseis::utils::MPI_type<unsigned char>(), rank + 1, 0, MPI_COMM_WORLD,
          &msg.back());
    }
    else if (
      old.global_offset + old.local_size
      < newd.global_offset + newd.local_size) {
        size_t sz = (newd.global_offset + newd.local_size)
                    - (old.global_offset + old.local_size);
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Irecv(
          vec->data() + vec->size() - sz, sz,
          exseis::utils::MPI_type<unsigned char>(), rank + 1, 1, MPI_COMM_WORLD,
          &msg.back());
    }

    MPI_Status stat;
    for (size_t i = 0; i < msg.size(); i++) {
        assert(msg[i] != MPI_REQUEST_NULL);
        int err = MPI_Wait(&msg[i], &stat);
        // TODO: Replace with standard approach to error handling
        if (err != MPI_SUCCESS) {
            std::cerr << "Wait " << i << std::endl;
            std::cerr << " MPI Error " << stat.MPI_ERROR << std::endl;
            std::exit(-1);
        }
    }

    // Cases 1, 4, 7
    if (old.global_offset < newd.global_offset) {
        size_t decStart = newd.global_offset - old.global_offset;
        std::move(vec->begin() + decStart, vec->end(), vec->begin());
    }

    vec->resize(newd.local_size);
}

// Write an arbitrary parallelised block sz to an arbitrary offset with the
// minimal block contention possible between processes.
// writeArb(out, hosz + decSz*i + (fsz - hosz) * (j+1), decSz, &out)
// off is the global offset
Contiguous_decomposition writeArb(
  size_t rank,
  size_t numRank,
  DataInterface* out,
  size_t off,
  size_t bsz,
  Contiguous_decomposition dec,
  size_t tsz,
  std::vector<unsigned char>* vec)
{
    auto newdec = blockDecomp(tsz, bsz, numRank, rank, off);

    // If there is one rank this is pointless
    if (numRank != 1) {
        // Reorder operations along new boundaries
        distribToDistrib(rank, dec, newdec, vec);
    }

    out->write(off + newdec.global_offset, newdec.local_size, vec->data());

    return newdec;
}

/*! Make a SEGY file by copying the header object and the data payload of an
 *  existing file and writng the header object and then repeatedly writing the
 *  payload.
 *  @param[in] piol The piol object
 *  @param[in] iname The SEG-Y input file name
 *  @param[in] oname The SEG-Y output file name
 *  @param[in] repRate The repetition rate
 */
void mpiMakeSEGYCopy(
  const ExSeis& piol, DataInterface* in, DataInterface* out, size_t repRate)
{
    size_t rank    = piol.getRank();
    size_t numRank = piol.getNumRank();

    const size_t fsz = in->getFileSz();
    piol.isErr();

    const size_t bsz  = 2097152LU;
    const size_t hosz = SEGY_utils::getHOSz();
    size_t memlim     = 512U * bsz;
    size_t step       = numRank * memlim;

    for (size_t i = 0; i < fsz; i += step) {
        size_t rblock = (i + step < fsz ? step : fsz - i);
        auto dec      = blockDecomp(rblock, bsz, numRank, rank, i);

        std::vector<unsigned char> buf(dec.local_size);
        in->read(i + dec.global_offset, dec.local_size, buf.data());
        piol.isErr();
        out->write(i + dec.global_offset, dec.local_size, buf.data());
        piol.isErr();
        if (i == 0) {
            // If zero, then current process has read the header object
            if (dec.global_offset == 0) {
                std::move(buf.begin() + hosz, buf.end(), buf.begin());
                dec.local_size -= hosz;
                buf.resize(dec.local_size);
            }
            else {
                dec.global_offset -= hosz;
            }

            rblock -= hosz;
        }
        size_t rank    = piol.getRank();
        size_t numRank = piol.getNumRank();
        for (size_t j = 1; j < repRate; j++) {
            dec = writeArb(
              rank, numRank, out, hosz + (fsz - hosz) * j + i, bsz, dec, rblock,
              &buf);
        }
    }
}

template<bool Block>
void mpiMakeSEGYCopyNaive(
  const ExSeis& piol, DataInterface* in, DataInterface* out, size_t repRate)
{
    size_t numRank    = piol.getNumRank();
    const size_t fsz  = in->getFileSz();
    const size_t bsz  = 2097152LU;
    const size_t hosz = SEGY_utils::getHOSz();
    size_t memlim     = 512U * bsz;
    size_t step       = numRank * memlim;

    for (size_t i = 0; i < fsz; i += step) {
        size_t rblock = (i + step < fsz ? step : fsz - i);
        auto dec =
          (Block ? block_decomposition(rblock, numRank, piol.getRank()) :
                   blockDecomp(rblock, bsz, numRank, piol.getRank(), i));

        std::vector<unsigned char> buf(dec.local_size);
        in->read(i + dec.global_offset, dec.local_size, buf.data());
        out->write(i + dec.global_offset, dec.local_size, buf.data());
        if (i == 0) {
            // If zero, then current process has read the header object
            if (dec.global_offset == 0) {
                std::move(
                  buf.begin() + hosz, buf.begin() + dec.local_size,
                  buf.begin());
                dec.local_size -= hosz;
                buf.resize(dec.local_size);
            }
            else {
                dec.global_offset -= hosz;
            }
        }
        for (size_t j = 1; j < repRate; j++) {
            out->write(
              hosz + (fsz - hosz) * j + i + dec.global_offset, dec.local_size,
              buf.data());
        }
    }
}

enum Version : size_t { Block, Naive1, Naive2 };

size_t getVersion(std::string version)
{
    std::unordered_map<std::string, Version> val = {
      {"standard", Version::Block},
      {"naive1", Version::Naive1},
      {"naive2", Version::Naive2}};
    return val.find(version)->second;
}

int main(int argc, char** argv)
{
    std::string opt = "i:o:v:r:";  // TODO: uses a GNU extension
    std::string iname;
    std::string oname;
    size_t rep     = 1;
    size_t version = size_t(Version::Block);
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
        switch (c) {
            case 'i':
                iname = optarg;
                break;

            case 'o':
                oname = optarg;
                break;

            case 'v':
                version = getVersion(optarg);
                break;

            case 'r':
                rep = std::stoul(optarg);
                break;

            default:
                fprintf(
                  stderr, "One of the command line arguments is invalid\n");
                break;
        }
    }
    assert(!iname.empty() && !oname.empty());

    auto piol = ExSeis::New();

    size_t numRank = piol->getNumRank();

    DataMPIIO in(piol, iname, FileMode::Read);
    DataMPIIO out(piol, oname, FileMode::Write);
    piol->isErr();

    const size_t fsz = in.getFileSz();
    piol->isErr();
    if (fsz / numRank < SEGY_utils::getHOSz()) {
        smallCopy(*piol, &in, &out, rep);
    }
    else {
        switch (version) {
            case Version::Block:
                if (piol->getRank() == 0) {
                    std::cout << "Standard\n";
                }
                mpiMakeSEGYCopy(*piol, &in, &out, rep);
                break;

            case Version::Naive1:
                if (piol->getRank() == 0) {
                    std::cout << "Naive 1\n";
                }
                mpiMakeSEGYCopyNaive<true>(*piol, &in, &out, rep);
                break;

            default:
            case Version::Naive2:
                if (piol->getRank() == 0) {
                    std::cout << "Naive 2\n";
                }
                mpiMakeSEGYCopyNaive<false>(*piol, &in, &out, rep);
                break;
        }
    }

    return 0;
}
