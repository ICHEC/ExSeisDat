#include "exseis/utils/distributed_vector/Distributed_vector_mpi.hh"

#include "exseis/utils/communicator/Communicator_mpi.hh"

#include "exseis/utils/distributed_vector/Distributed_vector.test.hh"

#include "exseis/test/catch2.hh"


TEST_CASE(
    "Distributed_vector_mpi",
    "[Distributed_vector_mpi][Distributed_vector][MPI]")
{
    // size_t global_size = GENERATE(1, 2, 3, 4, 10, 1024);
    size_t global_size = GENERATE(1, 2, 10, 100);

    exseis::Communicator_mpi communicator(MPI_COMM_WORLD);

    auto distributed_vector =
        exseis::Distributed_vector_mpi<size_t>(communicator, global_size);


    test_distributed_vector(distributed_vector, global_size, communicator);
}


TEST_CASE(
    "Distributed_vector_mpi 2GB Limit",
    "[Distributed_vector_mpi][Distributed_vector][MPI][2GB Limit]")
{
    auto communicator = exseis::Communicator_mpi(MPI_COMM_WORLD);


    size_t distributed_vector_size = size_t(2) * 1024 * 1024 * 1024;

    auto distributed_vector = exseis::Distributed_vector_mpi<unsigned char>(
        communicator, distributed_vector_size);

    test_distributed_vector_2gb_limit(distributed_vector, communicator);
}
