////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @date March 2017
/// @brief
/// @details Perform an analysis of a single trace.
////////////////////////////////////////////////////////////////////////////////

#include "exseis/piol/file/Input_file_segy.hh"
#include "exseis/piol/io_driver/IO_driver_mpi.hh"
#include "exseis/utils/communicator/Communicator_mpi.hh"

#include <iostream>
#include <unistd.h>


/*! Main function for traceanalysis.
 *  @param[in] argc The number of input strings.
 *  @param[in] argv The array of input strings.
 *  @return zero on success, non-zero on failure
 *  @details Two command line options:
 *           -i \<inp\> : input file
 *           -t \<num\> : input trace number (default 0)
 */
int main(int argc, char** argv)
{
    auto communicator = exseis::Communicator_mpi{MPI_COMM_WORLD};

    std::string name;
    size_t tn       = 0LU;
    std::string opt = "i:t:";  // TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
        switch (c) {
            case 'i':
                name = optarg;
                break;

            case 't':
                tn = std::stoul(optarg);
                break;

            default:
                std::cerr << "One of the command line arguments is invalid\n";
                break;
        }
    }

    exseis::Input_file_segy file(
        exseis::IO_driver_mpi{communicator, name, exseis::File_mode_mpi::Read});

    exseis::Trace_metadata trace_metadata(file.trace_metadata_available(), 1LU);
    file.read_metadata(tn, 1LU, trace_metadata);

    if (communicator.get_rank() == 0) {
        std::cout << "x_src "
                  << trace_metadata.get_floating_point(
                         0LU, exseis::Trace_metadata_key::source_x)
                  << std::endl;
        std::cout << "y_src "
                  << trace_metadata.get_floating_point(
                         0LU, exseis::Trace_metadata_key::source_y)
                  << std::endl;
        std::cout << "x_rcv "
                  << trace_metadata.get_floating_point(
                         0LU, exseis::Trace_metadata_key::receiver_x)
                  << std::endl;
        std::cout << "y_rcv "
                  << trace_metadata.get_floating_point(
                         0LU, exseis::Trace_metadata_key::receiver_y)
                  << std::endl;
        std::cout << "xCmp "
                  << trace_metadata.get_floating_point(
                         0LU, exseis::Trace_metadata_key::cdp_x)
                  << std::endl;
        std::cout << "yCmp "
                  << trace_metadata.get_floating_point(
                         0LU, exseis::Trace_metadata_key::cdp_y)
                  << std::endl;

        std::cout << "il "
                  << trace_metadata.get_integer(
                         0LU, exseis::Trace_metadata_key::il)
                  << std::endl;
        std::cout << "xl "
                  << trace_metadata.get_integer(
                         0LU, exseis::Trace_metadata_key::xl)
                  << std::endl;
    }
    return 0;
}
