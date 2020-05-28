#include "filesegytest.hh"

const size_t largens = 1000U;
const size_t largent = 2000000U;
const size_t bigtns  = 32000U;
const size_t smallns = 261U;
const size_t smallnt = 400U;

// Read test of SEGY -> ObjectSEGY -> IO_driver_mpi
TEST_F(FileSEGYIntegRead, SEGYReadHO)
{
    nt = smallnt;
    ns = smallns;
    make_segy<false>(small_segy_file());

    piol->assert_ok();
    EXPECT_EQ(ns, file->read_ns());
    piol->assert_ok();
    EXPECT_EQ(nt, file->read_nt());
    piol->assert_ok();
    if (sizeof(exseis::utils::Floating_point) == sizeof(double)) {
        EXPECT_DOUBLE_EQ(double(20e-6), file->read_sample_interval());
    }
    else {
        EXPECT_FLOAT_EQ(float(20e-6), file->read_sample_interval());
    }
}

TEST_F(FileSEGYIntegRead, FileReadTraceSmall)
{
    nt = smallnt;
    ns = smallns;
    make_segy<false>(small_segy_file());
    read_trace_test<false, false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FileReadTraceWPrmSmall)
{
    nt = smallnt;
    ns = smallns;
    make_segy<false>(small_segy_file());
    read_trace_test<true, false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FileReadRandomTraceSmall)
{
    nt           = smallnt;
    ns           = smallns;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy<false>(small_segy_file());
    read_random_trace_test<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FileReadRandomTraceWPrmSmall)
{
    nt           = smallnt;
    ns           = smallns;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy<false>(small_segy_file());
    read_random_trace_test<true, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FileReadTraceSmallOpts)
{
    nt = smallnt;
    ns = smallns;
    make_segy<true>(small_segy_file());
    read_trace_test<false, false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FileReadTraceWPrmSmallOpts)
{
    nt = smallnt;
    ns = smallns;
    make_segy<true>(small_segy_file());
    read_trace_test<true, false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FileReadRandomTraceSmallOpts)
{
    nt           = smallnt;
    ns           = smallns;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy<false>(small_segy_file());
    read_random_trace_test<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FileReadRandomTraceWPrmSmallOpts)
{
    nt           = smallnt;
    ns           = smallns;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy<false>(small_segy_file());
    read_random_trace_test<true, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FileReadTraceBigNS)
{
    nt = 200;
    ns = bigtns;
    make_segy<false>(big_trace_segy_file());
    read_trace_test<false, false>(10, nt);
}

TEST_F(FileSEGYIntegRead, FileReadTraceWPrmBigNS)
{
    nt = 200;
    ns = bigtns;
    make_segy<false>(big_trace_segy_file());
    read_trace_test<true, false>(10, nt);
}

TEST_F(FileSEGYIntegRead, FileReadRandomTraceBigNS)
{
    nt           = 200;
    ns           = bigtns;
    size_t size  = 2;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy<false>(big_trace_segy_file());
    read_random_trace_test<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FileReadRandomTraceWPrmBigNS)
{
    nt           = 200;
    ns           = bigtns;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy<false>(big_trace_segy_file());
    read_random_trace_test<true, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FileReadTraceBigOffset)
{
    nt = 10;
    ns = largens;
    make_segy<false>(large_segy_file());
    read_trace_test<false, false>(1999990U, nt);
}

TEST_F(FileSEGYIntegRead, FileReadTraceWPrmBigOffset)
{
    nt = 10;
    ns = largens;
    make_segy<false>(large_segy_file());
    read_trace_test<true, false>(1999990U, nt);
}

TEST_F(FileSEGYIntegRead, FarmFileReadTraceBigNt)
{
    nt = largent;
    ns = largens;
    make_segy<false>(large_segy_file());
    read_trace_test<false, false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FarmFileReadTraceWPrmBigNt)
{
    nt = largent;
    ns = largens;
    make_segy<false>(large_segy_file());
    read_trace_test<true, false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FarmFileReadRandomTraceBigNt)
{
    nt           = largent;
    ns           = largens;
    size_t size  = nt / 2;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy<false>(large_segy_file());
    read_random_trace_test<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FarmFileReadRandomTraceWPrmBigNt)
{
    nt           = largent;
    ns           = largens;
    size_t size  = nt / 2;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy<false>(large_segy_file());
    read_random_trace_test<true, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FileReadTraceZeroNt)
{
    nt = 0U;
    ns = largens;
    make_segy<false>(large_segy_file());
    read_trace_test<false, false>(10, nt);
}

TEST_F(FileSEGYIntegRead, FileReadTraceWPrmZeroNt)
{
    nt = 0U;
    ns = largens;
    make_segy<false>(large_segy_file());
    read_trace_test<true, false>(10, nt);
}

TEST_F(FileSEGYIntegRead, FarmFileReadRandomTraceZeroNt)
{
    nt           = 0U;
    ns           = largens;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy<false>(large_segy_file());
    read_random_trace_test<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FarmFileReadRandomTraceWPrmZeroNt)
{
    nt           = 0U;
    ns           = largens;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy<false>(large_segy_file());
    read_random_trace_test<true, false>(size, offsets);
}
