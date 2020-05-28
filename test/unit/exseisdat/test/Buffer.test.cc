#include "exseisdat/test/Buffer.hh"

#include "exseisdat/utils/communicator/Communicator_mpi.hh"

#include "exseisdat/test/catch2.hh"

TEST_CASE("Buffer")
{
    using Buffer = exseis::test::Buffer<size_t>;

    auto communicator =
        exseis::utils::Communicator_mpi{nullptr, {MPI_COMM_WORLD}};

    REQUIRE(exseis::test::total_global_buffer_usage() == 0);

    SECTION ("Memory usage should be globally tracked") {
        const size_t test_buffer_size = 10 * (communicator.get_rank() + 1);

        const auto buffer = Buffer(communicator, test_buffer_size);
        REQUIRE(buffer.size() == test_buffer_size);

        // Expect the global buffer usage to be the sum of the test_buffer_sizes
        const size_t expected_size =
            communicator.sum(test_buffer_size * sizeof(Buffer::value_type));
        REQUIRE(exseis::test::total_global_buffer_usage() == expected_size);

        // Adding an extra buffer should update it again
        auto buffer_2 = Buffer(communicator, 2 * test_buffer_size);
        REQUIRE(buffer_2.size() == 2 * test_buffer_size);

        REQUIRE(exseis::test::total_global_buffer_usage() == 3 * expected_size);

        // Moving a new buffer into the old one should remove the old tracked
        // size and add the new one
        buffer_2 = Buffer(communicator, 3 * test_buffer_size);
        REQUIRE(buffer_2.size() == 3 * test_buffer_size);
        REQUIRE(exseis::test::total_global_buffer_usage() == 4 * expected_size);

        // Move construction of the buffer should have no effect on the
        // global size
        Buffer buffer_3{std::move(buffer_2)};
        REQUIRE(buffer_3.size() == 3 * test_buffer_size);
        REQUIRE(exseis::test::total_global_buffer_usage() == 4 * expected_size);

        // Reconstructing a moved from Buffer should work as expected
        buffer_2 = Buffer(communicator, 4 * test_buffer_size);
        REQUIRE(buffer_2.size() == 4 * test_buffer_size);
        REQUIRE(exseis::test::total_global_buffer_usage() == 8 * expected_size);

        // Moving and deleting the buffer should remove the tracked size
        const auto sink = [](Buffer) {};
        sink(std::move(buffer_2));
        REQUIRE(exseis::test::total_global_buffer_usage() == 4 * expected_size);
    }
}
