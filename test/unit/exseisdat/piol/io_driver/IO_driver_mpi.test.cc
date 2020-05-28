#include "exseisdat/piol/io_driver/IO_driver_mpi.hh"

#include "exseisdat/piol/configuration/ExSeis.hh"

#include "exseisdat/piol/io_driver/IO_driver.test.hh"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <memory>
#include <random>
#include <vector>

#include <mpi.h>

#include <assert.h>
#if defined(NDEBUG)
#undef NDEBUG
#include <assert.h>
#define NDEBUG
#endif  // defined(NDEBUG)

#include "exseisdat/test/Test_case_cache.hh"
#include "exseisdat/test/catch2.hh"


class Inspector_io_driver_mpi {
  public:
    using IO_driver_mpi = exseis::piol::IO_driver_mpi;

    static exseis::utils::Communicator_mpi communicator()
    {
        return {nullptr, exseis::utils::Communicator_mpi::Opt{MPI_COMM_WORLD}};
    }

    // From Hash Prospector by Chris Wellons
    // https://nullprogram.com/blog/2018/07/31/
    static unsigned char pattern(uint64_t x)
    {
        x ^= x >> 30;
        x *= UINT64_C(0xbf58476d1ce4e5b9);
        x ^= x >> 27;
        x *= UINT64_C(0x94d049bb133111eb);
        x ^= x >> 31;
        return static_cast<unsigned char>(x);
    }

    static unsigned char alt_pattern(uint64_t x)
    {
        constexpr uint64_t x_max = std::numeric_limits<uint64_t>::max();
        return pattern(x_max - x);
    }

    static exseis::piol::IO_driver_mpi set_pattern(
        exseis::piol::IO_driver_mpi&& io_driver_mpi)
    {
        const size_t file_size =
            Inspector_io_driver_mpi::file_size(io_driver_mpi);

        const size_t one_mb         = 1024 * 1024;
        const size_t max_chunk_size = std::min<size_t>(
            {one_mb, exseis::test::remaining_global_buffer_space(), file_size});

        int rank = -1;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        const size_t local_buffer_size = rank == 0 ? max_chunk_size : 0;

        exseis::test::Buffer<unsigned char> buffer(
            communicator(), local_buffer_size);

        for (size_t chunk_start = 0; chunk_start < file_size;
             chunk_start += max_chunk_size) {
            // Rank 0 only
            if (rank != 0) continue;

            const size_t chunk_end =
                std::min<size_t>(chunk_start + max_chunk_size, file_size);
            const size_t chunk_size = chunk_end - chunk_start;

            for (size_t i = 0; i < chunk_size; i++) {
                buffer[i] = pattern(i + chunk_start);
            }

            MPI_File_write_at(
                io_driver_mpi.file_handle(), chunk_start, buffer.data(),
                chunk_size, MPI_CHAR, MPI_STATUS_IGNORE);
        }

        MPI_File_sync(io_driver_mpi.file_handle());
        MPI_Barrier(MPI_COMM_WORLD);

        return std::move(io_driver_mpi);
    }

    template<typename Callback>
    static void for_each(
        const exseis::piol::IO_driver_mpi& io_driver, Callback&& callback)
    {
        all_of(io_driver, [&](auto... args) {
            callback(args...);
            return true;
        });
    }

    template<typename Callback>
    static bool all_of(
        const exseis::piol::IO_driver_mpi& io_driver_mpi, Callback&& callback)
    {
        int number_of_ranks = -1;
        MPI_Comm_size(MPI_COMM_WORLD, &number_of_ranks);

        assert(
            exseis::test::remaining_global_buffer_space()
                >= static_cast<unsigned int>(number_of_ranks)
            && "Not enough global buffer space to allocate local buffer.");

        const size_t file_size =
            Inspector_io_driver_mpi::file_size(io_driver_mpi);

        constexpr size_t four_mb    = 4 * 1024 * 1024;
        const size_t max_chunk_size = std::min<size_t>(
            {four_mb,
             exseis::test::remaining_global_buffer_space() / number_of_ranks,
             file_size});

        exseis::test::Buffer<unsigned char> buffer(
            communicator(), max_chunk_size);

        bool success = true;

        int err = MPI_File_set_view(
            io_driver_mpi.file_handle(), 0, MPI_CHAR, MPI_CHAR, "native",
            MPI_INFO_NULL);
        REQUIRE(err == MPI_SUCCESS);
        for (size_t global_i = 0; global_i < file_size;
             global_i += max_chunk_size) {

            const size_t chunk_start = std::min(global_i, file_size);
            const size_t chunk_end =
                std::min(global_i + max_chunk_size, file_size);
            const size_t chunk_size = chunk_end - chunk_start;

            int read_err = MPI_File_read_at_all(
                io_driver_mpi.file_handle(), chunk_start, buffer.data(),
                buffer.size(), MPI_CHAR, MPI_STATUS_IGNORE);
            REQUIRE(read_err == MPI_SUCCESS);

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

    static exseis::piol::IO_driver_mpi set_file_size(
        exseis::piol::IO_driver_mpi&& io_driver_mpi, size_t file_size)
    {
        MPI_File file_handle = io_driver_mpi.file_handle();
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_File_sync(file_handle);
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Offset size = static_cast<MPI_Offset>(file_size);
        MPI_File_set_size(file_handle, size);

        return std::move(io_driver_mpi);
    }

    static size_t file_size(const exseis::piol::IO_driver_mpi& io_driver_mpi)
    {
        MPI_File file_handle = io_driver_mpi.file_handle();
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_File_sync(file_handle);
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Offset size = std::numeric_limits<MPI_Offset>::max();
        MPI_File_get_size(file_handle, &size);
        return static_cast<size_t>(size);
    }
};


TEST_CASE("Inspector_io_driver_mpi", "[IO_driver_mpi][IO_driver][PIOL]")
{
    constexpr const char* file_name = "Inspector_io_driver_mpi.test.dat";

    int rank = -1;
    int err  = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    REQUIRE(err == MPI_SUCCESS);

    if (rank == 0) {
        err = MPI_File_delete(file_name, MPI_INFO_NULL);

        // Acceptable errors in the class MPI_ERR_NO_SUCH_FILE
        if (err != MPI_SUCCESS) {
            int err_class = -1;
            err           = MPI_Error_class(err, &err_class);
            REQUIRE(err == MPI_SUCCESS);
            REQUIRE(err_class == MPI_ERR_NO_SUCH_FILE);
        }
    }

    auto piol = exseis::piol::ExSeis::make(
        exseis::utils::Verbosity::none, MPI_COMM_WORLD);
    exseis::piol::IO_driver_mpi io_driver_mpi{
        file_name, exseis::piol::File_mode_mpi::ReadWrite, MPI_COMM_WORLD,
        piol->log};

    // IO_driver_mpi should have a non-null file handle
    MPI_File file_handle = io_driver_mpi.file_handle();
    REQUIRE(file_handle != MPI_FILE_NULL);

    // IO_driver_mpi should initialize a zero-length file
    MPI_Offset file_size = std::numeric_limits<MPI_Offset>::max();
    err                  = MPI_File_get_size(file_handle, &file_size);
    REQUIRE(err == MPI_SUCCESS);
    REQUIRE(file_size == 0);

    // Test Inspector_io_driver_mpi re-initializes the IO_driver_mpi correctly
    for (const MPI_Offset new_file_size :
         {0, 1, 2, 10, 100, 0, 1, 2, 10, 100}) {
        // A buffer for reading the data. Prepared here to avoid adding
        // sync later.
        exseis::test::Buffer<unsigned char> buffer{
            *(piol->comm), static_cast<size_t>(new_file_size)};
        err = MPI_Barrier(MPI_COMM_WORLD);
        REQUIRE(err == MPI_SUCCESS);

        io_driver_mpi = Inspector_io_driver_mpi::set_file_size(
            std::move(io_driver_mpi), new_file_size);
        io_driver_mpi =
            Inspector_io_driver_mpi::set_pattern(std::move(io_driver_mpi));

        // The underlying size should match the requested size
        file_size = std::numeric_limits<MPI_Offset>::max();
        err       = MPI_File_get_size(file_handle, &file_size);
        REQUIRE(err == MPI_SUCCESS);
        REQUIRE(file_size == new_file_size);

        // Reading should be ready without synchronization etc.
        err = MPI_File_read_at(
            file_handle, 0, buffer.data(), buffer.size(), MPI_CHAR,
            MPI_STATUS_IGNORE);
        REQUIRE(err == MPI_SUCCESS);

        // The underlying file should be set to the pattern value
        for (size_t i = 0; i < buffer.size(); i++) {
            REQUIRE(buffer[i] == Inspector_io_driver_mpi::pattern(i));
        }

        // The all_of command should match the read data
        Inspector_io_driver_mpi::all_of(
            io_driver_mpi, [&](size_t index, unsigned char original_value,
                               unsigned char value) {
                REQUIRE(index < buffer.size());
                REQUIRE(original_value == buffer[index]);
                REQUIRE(value == buffer[index]);

                return true;
            });
    }

    err = MPI_File_sync(file_handle);
    REQUIRE(err == MPI_SUCCESS);

    const auto sink = [](exseis::piol::IO_driver_mpi) {};
    sink(std::move(io_driver_mpi));

    if (rank == 0) {
        err = MPI_File_delete(file_name, MPI_INFO_NULL);
        REQUIRE(err == MPI_SUCCESS);
    }
}


// Make sure file doesn't exist before IO_driver_mpi is called,
// and deleted after IO_driver_mpi is deleted.
struct Manage_file {
    const char* file_name = "IO_driver_mpi.test.dat";

    void delete_file()
    {
        MPI_Barrier(MPI_COMM_WORLD);

        int rank = -1;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if (rank == 0) {
            int err = MPI_File_delete(file_name, MPI_INFO_NULL);

            // The only acceptable error is one in the class
            // MPI_ERR_NO_SUCH_FILE
            if (err != MPI_SUCCESS) {
                int err_class = -1;
                err           = MPI_Error_class(err, &err_class);
                assert(err == MPI_SUCCESS);
                assert(err_class == MPI_ERR_NO_SUCH_FILE);
            }
        }
    }

    Manage_file() { delete_file(); }

    ~Manage_file() { delete_file(); }
};
struct IO_driver_cache : public Manage_file {

    std::shared_ptr<exseis::piol::ExSeis> piol = exseis::piol::ExSeis::make(
        exseis::utils::Verbosity::none, MPI_COMM_WORLD);

    exseis::piol::IO_driver_mpi io_driver_mpi{
        file_name, exseis::piol::File_mode_mpi::ReadWrite, MPI_COMM_WORLD,
        piol->log};

    size_t max_io_chunk_size =
        exseis::piol::IO_driver_mpi::Options{}.max_io_chunk_size;
};


TEST_CASE("IO_driver_mpi", "[IO_driver_mpi][IO_driver][PIOL]")
{
    // Set max I/O chunk size
    enum class Io_chunk_mode { Default, Small };

    auto io_chunk_mode = GENERATE(Io_chunk_mode::Default, Io_chunk_mode::Small);

    size_t file_size = GENERATE(1, 1000);

    const size_t max_io_chunk_size = ([=] {
        switch (io_chunk_mode) {
            default:
            case Io_chunk_mode::Default:
                return exseis::piol::IO_driver_mpi::Options{}.max_io_chunk_size;
            case Io_chunk_mode::Small:
                return std::max<size_t>(1, file_size / 4);
        }
    }());

    auto& cache = exseis::test::Test_case_cache::get<IO_driver_cache>();


    // Re-initialize cached IO_driver_mpi if necessary
    if (cache.max_io_chunk_size != max_io_chunk_size) {
        exseis::piol::IO_driver_mpi::Options options{};
        options.max_io_chunk_size = max_io_chunk_size;

        cache.io_driver_mpi = exseis::piol::IO_driver_mpi{
            cache.file_name, exseis::piol::File_mode_mpi::ReadWrite,
            MPI_COMM_WORLD, cache.piol->log, options};

        cache.max_io_chunk_size = max_io_chunk_size;
    }
    auto& io_driver_mpi = cache.io_driver_mpi;

    io_driver_mpi = Inspector_io_driver_mpi::set_file_size(
        std::move(io_driver_mpi), file_size);

    auto communicator = Inspector_io_driver_mpi::communicator();

    test_io_driver<Inspector_io_driver_mpi>(io_driver_mpi, communicator);
}

TEST_CASE(
    "IO_driver_mpi 2GB Limit", "[IO_driver_mpi][IO_driver][PIOL][2GB Limit]")
{
    auto piol = exseis::piol::ExSeis::make(
        exseis::utils::Verbosity::none, MPI_COMM_WORLD);
    static const char* file_name = "IO_driver_mpi_2GB_limit.test.dat";
    auto file_mode               = exseis::piol::File_mode_mpi::ReadWrite;

    size_t file_size = size_t(2) * 1024 * 1024 * 1024;

    exseis::piol::IO_driver_mpi io_driver_mpi(
        file_name, file_mode, MPI_COMM_WORLD, piol->log);
    io_driver_mpi.set_file_size(file_size);

    REQUIRE(io_driver_mpi.get_file_size() == file_size);

    test_io_driver_2gb_limit(io_driver_mpi, *(piol->comm));


    // Close the file and delete it
    const auto sink = [](exseis::piol::IO_driver_mpi) {};
    sink(std::move(io_driver_mpi));

    MPI_Barrier(MPI_COMM_WORLD);
    int rank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        int err = MPI_File_delete(file_name, MPI_INFO_NULL);
        REQUIRE(err == MPI_SUCCESS);
    }
    MPI_Barrier(MPI_COMM_WORLD);
}
