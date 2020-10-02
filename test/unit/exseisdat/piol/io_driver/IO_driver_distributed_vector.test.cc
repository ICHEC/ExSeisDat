#include "exseisdat/piol/io_driver/IO_driver_distributed_vector.hh"
#include "exseisdat/utils/distributed_vector/Distributed_vector_mpi.hh"

#include "exseisdat/piol/io_driver/IO_driver.test.hh"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <random>
#include <vector>

#include <mpi.h>

#if defined(NDEBUG)
#undef NDEBUG
#include <assert.h>
#define NDEBUG
#endif  // defined(NDEBUG)

#include "exseisdat/test/Index_hash.hh"
#include "exseisdat/test/Test_case_cache.hh"
#include "exseisdat/test/catch2.hh"

namespace {

class Inspector_io_driver_distributed_vector {
  public:
    using Distributed_vector = exseis::utils::Distributed_vector<unsigned char>;

    static int get_rank()
    {
        int rank = -1;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        return rank;
    }

    static exseis::utils::Communicator_mpi communicator()
    {
        return {nullptr, exseis::utils::Communicator_mpi::Opt{MPI_COMM_WORLD}};
    }

    static unsigned char pattern(uint64_t x)
    {
        return exseis::test::Index_hash::get<unsigned char>(x);
    }

    static unsigned char alt_pattern(uint64_t x)
    {
        constexpr uint64_t x_max = std::numeric_limits<uint64_t>::max();
        return pattern(x_max - x);
    }

    static Distributed_vector make_storage(size_t file_size)
    {
        assert(exseis::test::remaining_global_buffer_space() >= file_size);
        return exseis::utils::Distributed_vector_mpi<unsigned char>{
            file_size, MPI_COMM_WORLD};
    }

    static exseis::piol::IO_driver_distributed_vector set_pattern(
        exseis::piol::IO_driver_distributed_vector&&
            io_driver_distributed_vector)
    {
        Distributed_vector storage =
            std::move(io_driver_distributed_vector).distributed_vector();
        storage = set_pattern(std::move(storage));
        return {std::move(storage)};
    }

    static Distributed_vector set_pattern(Distributed_vector&& storage)
    {
        const size_t file_size = storage.size();

        int rank = -1;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        constexpr size_t four_mb    = 4 * 1024 * 1024;
        const size_t max_chunk_size = std::min<size_t>(
            {four_mb, exseis::test::remaining_global_buffer_space(),
             file_size});

        const size_t buffer_size = rank == 0 ? max_chunk_size : 0;
        exseis::test::Buffer<unsigned char> buffer(communicator(), buffer_size);

        for (size_t global_i = 0; global_i < file_size;
             global_i += max_chunk_size) {

            const size_t chunk_start = std::min<size_t>(global_i, file_size);
            const size_t chunk_end =
                std::min<size_t>(chunk_start + max_chunk_size, file_size);
            const size_t chunk_size = chunk_end - chunk_start;

            if (rank == 0) {
                for (size_t i = 0; i < chunk_size; i++) {
                    buffer[i] = pattern(chunk_start + i);
                }

                storage.set_n(chunk_start, chunk_size, buffer.data());
            }
        }

        storage.sync();

        return std::move(storage);
    }

    template<typename Callback>
    static void for_each(
        const exseis::piol::IO_driver_distributed_vector&
            io_driver_distributed_vector,
        Callback&& callback)
    {
        all_of(io_driver_distributed_vector, [&](auto... args) {
            callback(args...);
            return true;
        });
    }

    template<typename Callback>
    static bool all_of(
        const exseis::piol::IO_driver_distributed_vector&
            io_driver_distributed_vector,
        Callback&& callback)
    {
        size_t file_size = Inspector_io_driver_distributed_vector::file_size(
            io_driver_distributed_vector);

        int number_of_ranks = -1;
        MPI_Comm_size(MPI_COMM_WORLD, &number_of_ranks);

        assert(
            exseis::test::remaining_global_buffer_space()
                >= static_cast<unsigned int>(number_of_ranks)
            && "Not enough global buffer space to allocate local buffer.");

        const size_t local_max_buffer_size =
            exseis::test::remaining_global_buffer_space() / number_of_ranks;

        const size_t max_chunk_size =
            std::min<size_t>(local_max_buffer_size, file_size);

        const size_t buffer_size = max_chunk_size;
        exseis::test::Buffer<unsigned char> buffer(communicator(), buffer_size);

        bool success = true;

        for (size_t global_i = 0; global_i < file_size;
             global_i += max_chunk_size) {

            const size_t chunk_start = std::min(global_i, file_size);
            const size_t chunk_end =
                std::min(global_i + max_chunk_size, file_size);
            const size_t chunk_size = chunk_end - chunk_start;

            io_driver_distributed_vector.distributed_vector().get_n(
                chunk_start, chunk_size, buffer.data());

            for (size_t chunk_data_index = 0; chunk_data_index < chunk_size;
                 chunk_data_index++) {
                const size_t index = chunk_start + chunk_data_index;

                success = success
                          && callback(
                              index, pattern(index), buffer[chunk_data_index]);
            }
        }

        return success;
    }

    static size_t file_size(const exseis::piol::IO_driver_distributed_vector&
                                io_driver_distributed_vector)
    {
        return io_driver_distributed_vector.distributed_vector().size();
    }
};


struct Cache {
    exseis::utils::Distributed_vector<unsigned char> distributed_vector =
        Inspector_io_driver_distributed_vector::make_storage(0);
};

}  // namespace

TEST_CASE(
    "IO_driver_distributed_vector",
    "[IO_driver_distributed_vector][IO_driver][Distributed_vector][PIOL]")
{
    auto communicator = Inspector_io_driver_distributed_vector::communicator();

    size_t file_size = GENERATE(1, 100);

    // Get the Distributed_vector_mpi from cache
    auto& cache = exseis::test::Test_case_cache::get<Cache>();
    if (cache.distributed_vector.size() != file_size) {
        cache.distributed_vector =
            Inspector_io_driver_distributed_vector::make_storage(file_size);
    }
    REQUIRE(cache.distributed_vector.size() == file_size);

    exseis::piol::IO_driver_distributed_vector io_driver_distributed_vector{
        std::move(cache.distributed_vector)};

    test_io_driver<Inspector_io_driver_distributed_vector>(
        io_driver_distributed_vector, communicator);

    // Put the Distributed_vector_mpi back into cache
    cache.distributed_vector =
        std::move(io_driver_distributed_vector).distributed_vector();
}

TEST_CASE(
    "IO_driver_distributed_vector 2GB Limit",
    "[IO_driver_distributed_vector][IO_driver][Distributed_vector][PIOL][2GB Limit]")
{
    auto communicator = Inspector_io_driver_distributed_vector::communicator();

    size_t file_size = size_t(2) * 1024 * 1024 * 1024;

    using Distributed_vector =
        exseis::utils::Distributed_vector_mpi<unsigned char>;

    exseis::piol::IO_driver_distributed_vector io_driver(
        Distributed_vector(file_size, MPI_COMM_WORLD));

    test_io_driver_2gb_limit(io_driver, communicator);
}
