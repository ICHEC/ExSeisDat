#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

#include "exseisdat/piol/configuration/ExSeis.hh"

#include <cstdio>
#include <mpi.h>

using namespace testing;
using namespace exseis::piol;

// Number less than 256 that isn't 0.
const size_t magic_num1      = 137;
const size_t small_size      = 4U * prefix(1);
const size_t large_size      = 10U * prefix(3);
const size_t large_segy_size = (4U * 1000U + 240U) * 2000000U + 3600U;

std::string test_data_directory;

std::string zero_file()
{
    return test_data_directory + "/zeroSizeFile.tmp";
}
std::string small_file()
{
    return test_data_directory + "/smallSizeFile.tmp";
}
std::string large_file()
{
    return test_data_directory + "/largeSizeFile.tmp";
}
std::string plarge_file()
{
    return test_data_directory + "/large_file_pattern.tmp";
}
std::string temp_file()
{
    return test_data_directory + "/temp_file.tmp";
}
std::string temp_file_segy()
{
    return test_data_directory + "/temp_file.segy";
}
std::string small_segy_file()
{
    return test_data_directory + "/small_segy.tmp";
}
std::string large_segy_file()
{
    return test_data_directory + "/large_segy.tmp";
}
std::string big_trace_segy_file()
{
    return test_data_directory + "/big_trace_segy.tmp";
}

std::string nonexistant_filename()
{
    std::string filename;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
// Get unique name from tmpnam.
// We're silencing the depreciated warning because we aren't
// worried about security here
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        filename = tmpnam(nullptr);
#pragma GCC diagnostic pop

        // Get the base filename by taking everything after the last "\" or "/"
        filename = filename.substr(filename.find_last_of("/\\") + 1);

        // Prepend the test data directory and "should-not-exist-"
        filename = test_data_directory + "/should-not-exist-" + filename;
    }

    // Broadcast filename size
    int filename_size = filename.size();
    MPI_Bcast(&filename_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    filename.resize(filename_size);

    // Broadcast filename (via std::vector)
    auto filename_data = std::vector<char>(filename_size + 1);
    std::copy(
        std::begin(filename), std::end(filename), std::begin(filename_data));
    MPI_Bcast(
        filename_data.data(), filename_data.size(), MPI_CHAR, 0,
        MPI_COMM_WORLD);
    std::copy(
        std::begin(filename_data), std::end(filename_data),
        std::begin(filename));

    return filename;
}

int32_t il_num(size_t i)
{
    return 1600L + (i / 3000L);
}

int32_t xl_num(size_t i)
{
    return 1600L + (i % 3000L);
}

exseis::utils::Floating_point x_num(size_t i)
{
    return 1000L + (i / 2000L);
}

exseis::utils::Floating_point y_num(size_t i)
{
    return 1000L + (i % 2000L);
}

void make_file(std::string name, size_t sz)
{
    static const char zero = '\0';
    FILE* fs               = fopen(name.c_str(), "w");

    if (sz != 0) {
        // Seek beyond the end of the file and write a single null byte. This
        // ensures the file is all zeroes according to IEEE Std 1003.1-2013
        fseek(fs, sz - 1ll, SEEK_SET);
        fwrite(&zero, sizeof(unsigned char), 1, fs);
    }
    fclose(fs);
}

unsigned char get_pattern(size_t i)
{
    const size_t psz = 0x100;
    i %= psz;
    return i;
}

std::vector<size_t> get_random_vec(size_t nt, size_t max, int seed)
{
    srand(seed);
    if (nt == 0) {
        return std::vector<size_t>();
    }

    exseis::utils::Integer range = (max / nt) - 1LL;
    assert(range >= 0);

    std::vector<size_t> v(nt);
    v[0] = (range != 0 ? rand() % range : 0);
    for (size_t i = 1; i < nt; i++) {
        v[i] = v[i - 1] + 1U + (range != 0 ? rand() % range : 0);
    }
    return v;
}

std::vector<size_t> get_random_vec(size_t nt, int seed)
{
    return get_random_vec(nt, 12345, seed);
}


// Event Listener for GoogleTest that adds a global MPI barrier to the
// beginning and end of every test, improving the grouping of outputs.
class MPI_Barrier_listener : public ::testing::EmptyTestEventListener {
    void OnTestStart(const ::testing::TestInfo&) override { barrier_flush(); }

    void OnTestEnd(const ::testing::TestInfo&) override { barrier_flush(); }

    void barrier_flush() const
    {
        int initialized = 0;
        MPI_Initialized(&initialized);

        if (initialized == 1) {
            MPI_Barrier(MPI_COMM_WORLD);
            fflush(stdout);
            fflush(stderr);
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }
};

int main(int argc, char** argv)
{

    MPI_Init(&argc, &argv);

    auto piol = ExSeis::make();

    // First argument must be the test data directory
    test_data_directory = argv[1];
    if (piol->get_rank() == 0) {
        printf("Test Data Directory: %s\n", test_data_directory.c_str());
    }

    InitGoogleTest(&argc, argv);

    auto& listeners = ::testing::UnitTest::GetInstance()->listeners();

    // Add MPI Barrier to start and end of tests.
    listeners.Append(new MPI_Barrier_listener);

    // Put default listener after MPI Barrier listener
    auto* printer = listeners.default_result_printer();
    listeners.Release(printer);
    listeners.Append(printer);


    if (piol->get_rank() == 0) {
        make_file(zero_file(), 0U);
        make_file(small_file(), small_size);
        make_file(large_file(), large_size);
    }
    piol->barrier();

    int code = RUN_ALL_TESTS();

    piol->barrier();
    if (piol->get_rank() == 0) {
        std::remove(zero_file().c_str());
        std::remove(small_file().c_str());
        std::remove(large_file().c_str());
    }

    MPI_Finalize();

    return code;
}
