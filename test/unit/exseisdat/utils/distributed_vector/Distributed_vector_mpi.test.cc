#include "exseisdat/utils/distributed_vector/Distributed_vector_mpi.hh"

#include "exseisdat/utils/communicator/Communicator_mpi.hh"

#include "exseisdat/utils/distributed_vector/Distributed_vector.test.hh"

#include "exseisdat/test/catch2.hh"


using namespace exseis::utils;

TEST_CASE(
    "Distributed_vector_mpi",
    "[Distributed_vector_mpi][Distributed_vector][MPI]")
{
    // size_t global_size = GENERATE(1, 2, 3, 4, 10, 1024);
    size_t global_size = GENERATE(1, 2, 10, 100);

    auto distributed_vector =
        Distributed_vector_mpi<size_t>(global_size, MPI_COMM_WORLD);

    Communicator_mpi communicator(nullptr, {MPI_COMM_WORLD});

    test_distributed_vector(distributed_vector, global_size, communicator);
}


TEST_CASE(
    "Distributed_vector_mpi 2GB Limit",
    "[Distributed_vector_mpi][Distributed_vector][MPI][2GB Limit]")
{
    auto communicator = exseis::utils::Communicator_mpi(
        nullptr, exseis::utils::Communicator_mpi::Opt{MPI_COMM_WORLD});


    size_t distributed_vector_size = size_t(2) * 1024 * 1024 * 1024;

    auto distributed_vector =
        exseis::utils::Distributed_vector_mpi<unsigned char>(
            distributed_vector_size, MPI_COMM_WORLD);

    test_distributed_vector_2gb_limit(distributed_vector, communicator);
}
