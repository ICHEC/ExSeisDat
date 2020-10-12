#include "sglobal.hh"

#include "exseis/piol/file/Input_file_segy.hh"
#include "exseis/piol/io_driver/IO_driver_mpi.hh"
#include "exseis/piol/operations/sort.hh"
#include "exseis/utils/communicator/Communicator_mpi.hh"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unistd.h>


int main(int argc, char** argv)
{
    auto communicator = exseis::Communicator_mpi{MPI_COMM_WORLD};

    std::string opt = "i:o:t:d";  // TODO: uses a GNU extension
    std::string name1;
    exseis::Sort_type type = exseis::Sort_type::SrcRcv;
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
        switch (c) {
            case 'i':
                name1 = optarg;
                break;

            case 't':
                type = static_cast<exseis::Sort_type>(std::stoul(optarg));
                break;

            default:
                fprintf(
                    stderr, "One of the command line arguments is invalid\n");
                break;
        }
    }

    assert(!name1.empty());

    exseis::Input_file_segy src(exseis::IO_driver_mpi{
        communicator, name1, exseis::File_mode_mpi::Read});

    // Perform the decomposition and read coordinates of interest.
    auto dec = exseis::block_decomposition(
        src.read_number_of_traces(), communicator.get_num_rank(),
        communicator.get_rank());

    if (check_order(src, dec, type)) {
        std::cout << "Success\n";
    }
    else {
        std::cerr << "Failure\n";
    }

    return 0;
}
