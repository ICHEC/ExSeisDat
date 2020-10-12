#include "sglobal.hh"

#include "exseis/piol/io_driver/IO_driver_mpi.hh"
#include "exseis/piol/segy/utils.hh"
#include "exseis/utils/communicator/Communicator.hh"
#include "exseis/utils/types/MPI_type.hh"

#include <assert.h>
#include <iostream>
#include <unistd.h>
#include <unordered_map>


void printmsg(std::string msg, size_t sz, size_t rank, size_t rankn)
{
    std::cout << msg << sz << " " << rank << " " << rankn << std::endl;
}

void small_copy(
    const exseis::Communicator& communicator,
    exseis::IO_driver* in,
    exseis::IO_driver* out,
    size_t rep_rate)
{
    const size_t rank = communicator.get_rank();

    const size_t fsz  = in->get_file_size();
    const size_t hosz = exseis::segy::segy_binary_file_header_size();
    size_t wsz        = (rank == 0 ? fsz : 0);
    std::vector<unsigned char> buf(wsz);

    in->read(0, wsz, buf.data());

    out->write(0, wsz, buf.data());

    if (rank == 0) {
        std::copy(buf.begin() + hosz, buf.end(), buf.begin());
    }

    for (size_t j = 1; j < rep_rate; j++) {
        out->write(
            hosz + (fsz - hosz) * j, (rank == 0 ? fsz - hosz : 0), buf.data());
    }
}

void distrib_to_distrib(
    size_t rank,
    exseis::Contiguous_decomposition old,
    exseis::Contiguous_decomposition newd,
    std::vector<unsigned char>* vec)
{
    std::vector<MPI_Request> msg;

    // if the old offset is less than the new offset
    // Then the local process must give the data over to the
    //-1 lower ranked process

    // Cases 2, 3, 8
    if (old.global_offset > newd.global_offset) {
        size_t inc_start = old.global_offset - newd.global_offset;
        vec->resize(vec->size() + inc_start);
        // Making space for the move
        std::move_backward(vec->begin(), vec->end() - inc_start, vec->end());
    }

    if ((old.global_offset + old.local_size)
        < (newd.global_offset + newd.local_size)) {

        vec->resize(
            vec->size() + newd.global_offset + newd.local_size
            - (old.global_offset + old.local_size));
    }

    if (old.global_offset < newd.global_offset) {
        size_t sz = newd.global_offset - old.global_offset;
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Isend(
            vec->data(), sz, exseis::mpi_type<unsigned char>(), rank - 1, 1,
            MPI_COMM_WORLD, &msg.back());
    }
    else if (old.global_offset > newd.global_offset) {
        size_t sz = old.global_offset - newd.global_offset;
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Irecv(
            vec->data(), sz, exseis::mpi_type<unsigned char>(), rank - 1, 0,
            MPI_COMM_WORLD, &msg.back());
    }

    if (old.global_offset + old.local_size
        > newd.global_offset + newd.local_size) {
        size_t sz = old.global_offset + old.local_size
                    - (newd.global_offset + newd.local_size);
        msg.push_back(MPI_REQUEST_NULL);
        MPI_Isend(
            vec->data() + vec->size() - sz, sz,
            exseis::mpi_type<unsigned char>(), rank + 1, 0, MPI_COMM_WORLD,
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
            exseis::mpi_type<unsigned char>(), rank + 1, 1, MPI_COMM_WORLD,
            &msg.back());
    }

    MPI_Status stat;
    for (size_t i = 0; i < msg.size(); i++) {
        assert(msg[i] != MPI_REQUEST_NULL);
        int err = MPI_Wait(&msg[i], &stat);
        // TODO: Replace with standard approach to error handling
        if (err != MPI_SUCCESS) {
            std::cerr << "wait " << i << std::endl;
            std::cerr << " MPI Error " << stat.MPI_ERROR << std::endl;
            std::exit(-1);
        }
    }

    // Cases 1, 4, 7
    if (old.global_offset < newd.global_offset) {
        size_t dec_start = newd.global_offset - old.global_offset;
        std::move(vec->begin() + dec_start, vec->end(), vec->begin());
    }

    vec->resize(newd.local_size);
}

// Write an arbitrary parallelised block sz to an arbitrary offset with the
// minimal block contention possible between processes.
// write_arb(out, hosz + decSz*i + (fsz - hosz) * (j+1), decSz, &out)
// off is the global offset
exseis::Contiguous_decomposition write_arb(
    size_t rank,
    size_t num_rank,
    exseis::IO_driver* out,
    size_t off,
    size_t bsz,
    exseis::Contiguous_decomposition dec,
    size_t tsz,
    std::vector<unsigned char>* vec)
{
    auto newdec = exseis::apps::block_decomp(tsz, bsz, num_rank, rank, off);

    // If there is one rank this is pointless
    if (num_rank != 1) {
        // Reorder operations along new boundaries
        distrib_to_distrib(rank, dec, newdec, vec);
    }

    out->write(off + newdec.global_offset, newdec.local_size, vec->data());

    return newdec;
}

/*! Make a SEGY file by copying the header object and the data payload of an
 *  existing file and writng the header object and then repeatedly writing the
 *  payload.
 *  @param[in] communicator  The communicator the file is opened over
 *  @param[in] in The SEG-Y input file name
 *  @param[in] out The SEG-Y output file name
 *  @param[in] rep_rate The repetition rate
 */
void mpi_make_segy_copy(
    const exseis::Communicator& communicator,
    exseis::IO_driver* in,
    exseis::IO_driver* out,
    size_t rep_rate)
{
    size_t rank     = communicator.get_rank();
    size_t num_rank = communicator.get_num_rank();

    const size_t fsz = in->get_file_size();

    const size_t bsz  = 2097152LU;
    const size_t hosz = exseis::segy::segy_binary_file_header_size();
    size_t memlim     = 512U * bsz;
    size_t step       = num_rank * memlim;

    for (size_t i = 0; i < fsz; i += step) {
        size_t rblock = (i + step < fsz ? step : fsz - i);
        auto dec = exseis::apps::block_decomp(rblock, bsz, num_rank, rank, i);

        std::vector<unsigned char> buf(dec.local_size);
        in->read(i + dec.global_offset, dec.local_size, buf.data());
        out->write(i + dec.global_offset, dec.local_size, buf.data());
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

        for (size_t j = 1; j < rep_rate; j++) {
            dec = write_arb(
                rank, num_rank, out, hosz + (fsz - hosz) * j + i, bsz, dec,
                rblock, &buf);
        }
    }
}

template<bool Block>
void mpi_make_segy_copy_naive(
    const exseis::Communicator& communicator,
    exseis::IO_driver* in,
    exseis::IO_driver* out,
    size_t rep_rate)
{
    size_t num_rank   = communicator.get_num_rank();
    const size_t fsz  = in->get_file_size();
    const size_t bsz  = 2097152LU;
    const size_t hosz = exseis::segy::segy_binary_file_header_size();
    size_t memlim     = 512U * bsz;
    size_t step       = num_rank * memlim;

    for (size_t i = 0; i < fsz; i += step) {
        size_t rblock = (i + step < fsz ? step : fsz - i);
        auto dec =
            (Block ? exseis::block_decomposition(
                 rblock, num_rank, communicator.get_rank()) :
                     exseis::apps::block_decomp(
                         rblock, bsz, num_rank, communicator.get_rank(), i));

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
        for (size_t j = 1; j < rep_rate; j++) {
            out->write(
                hosz + (fsz - hosz) * j + i + dec.global_offset, dec.local_size,
                buf.data());
        }
    }
}

enum Version : size_t { Block, Naive1, Naive2 };

size_t get_version(std::string version)
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
                version = get_version(optarg);
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

    auto communicator = exseis::Communicator_mpi{MPI_COMM_WORLD};

    size_t num_rank = communicator.get_num_rank();

    exseis::IO_driver_mpi in(communicator, iname, exseis::File_mode_mpi::Read);
    exseis::IO_driver_mpi out(
        communicator, oname, exseis::File_mode_mpi::Write);

    const size_t fsz = in.get_file_size();
    if (fsz / num_rank < exseis::segy::segy_binary_file_header_size()) {
        small_copy(communicator, &in, &out, rep);
    }
    else {
        switch (version) {
            case Version::Block:
                if (communicator.get_rank() == 0) {
                    std::cout << "Standard\n";
                }
                mpi_make_segy_copy(communicator, &in, &out, rep);
                break;

            case Version::Naive1:
                if (communicator.get_rank() == 0) {
                    std::cout << "Naive 1\n";
                }
                mpi_make_segy_copy_naive<true>(communicator, &in, &out, rep);
                break;

            default:
            case Version::Naive2:
                if (communicator.get_rank() == 0) {
                    std::cout << "Naive 2\n";
                }
                mpi_make_segy_copy_naive<false>(communicator, &in, &out, rep);
                break;
        }
    }

    return 0;
}
