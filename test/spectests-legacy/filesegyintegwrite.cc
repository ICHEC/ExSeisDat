#include "filesegytest.hh"

// Write test of SEGY -> ObjectSEGY -> IO_driver_mpi
TEST_F(FileSEGYIntegWrite, SEGYWriteReadHO)
{
    ns = 261U;
    nt = 400U;

    make_segy();
    EXPECT_EQ(ns, readfile->read_ns());
    piol->assert_ok();

    EXPECT_EQ(nt, readfile->read_nt());
    piol->assert_ok();

    std::string text = readfile->read_text();
    piol->assert_ok();
    ASSERT_TRUE(test_string.size() <= text.size());
    for (size_t i = 0; i < test_string.size(); i++) {
        ASSERT_EQ(test_string[i], text[i]);
    }
}

// Write test of SEGY -> ObjectSEGY -> IO_driver_mpi
TEST_F(FileSEGYIntegWrite, SEGYWriteReadTrace_metadata)
{
    ns = 261U;
    nt = 400U;
    coord_t coord(1600, 2000);
    grid_t grid(il_num(201), xl_num(201));
    Trace_metadata prm(1U), prm2(1U);

    make_segy();

    piol->assert_ok();
    file->write_ns(ns);
    file->write_nt(nt);
    piol->assert_ok();

    prm.set_integer(0, Trace_metadata_key::il, grid.il);
    prm.set_integer(0, Trace_metadata_key::xl, grid.xl);
    prm.set_floating_point(0, Trace_metadata_key::xCmp, coord.x);
    prm.set_floating_point(0, Trace_metadata_key::yCmp, coord.y);

    file->write_param(201U, 1U, &prm);
    readfile->read_param(201U, 1U, &prm2);

    ASSERT_EQ(grid.il, prm2.get_integer(0U, Trace_metadata_key::il));
    ASSERT_EQ(grid.xl, prm2.get_integer(0U, Trace_metadata_key::xl));
    ASSERT_DOUBLE_EQ(
        coord.x, prm2.get_floating_point(0U, Trace_metadata_key::xCmp));
    ASSERT_DOUBLE_EQ(
        coord.y, prm2.get_floating_point(0U, Trace_metadata_key::yCmp));
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceNormal)
{
    nt = 100;
    ns = 300;
    make_segy();
    write_trace_test<false, false>(0, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceWPrmNormal)
{
    nt = 100;
    ns = 300;
    make_segy();
    write_trace_test<true, false>(0, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceNormal)
{
    nt           = 100;
    ns           = 300;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy();
    write_random_trace_test<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceWPrmNormal)
{
    nt           = 100;
    ns           = 300;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy();
    write_random_trace_test<true, false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceNormalOpt)
{
    nt = 100;
    ns = 300;
    make_segy();
    write_trace_test<false, false>(0, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceWPrmNormalOpt)
{
    nt = 100;
    ns = 300;
    make_segy();
    write_trace_test<true, false>(0, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceNormalOpt)
{
    nt           = 100;
    ns           = 300;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy();
    write_random_trace_test<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceWPrmNormalOpt)
{
    nt           = 100;
    ns           = 300;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy();
    write_random_trace_test<true, false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceBigNs)
{
    nt = 100;
    ns = 10000;
    make_segy();
    write_trace_test<false, false>(10, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceWPrmBigNs)
{
    nt = 100;
    ns = 10000;
    make_segy();
    write_trace_test<true, false>(10, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceBigNs)
{
    nt           = 100;
    ns           = 10000;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy();
    write_random_trace_test<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceWPrmBigNs)
{
    nt           = 100;
    ns           = 10000;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy();
    write_random_trace_test<true, false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceBigOffset)
{
    nt = 10;
    ns = 3000;
    make_segy();
    write_trace_test<false, false>(3728270, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceWPrmBigOffset)
{
    nt = 10;
    ns = 3000;
    make_segy();
    write_trace_test<true, false>(3728270, nt);
}

TEST_F(FileSEGYIntegWrite, FarmFileWriteTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    make_segy();
    write_trace_test<false, false>(0, nt);
}

TEST_F(FileSEGYIntegWrite, FarmFileWriteTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    make_segy();
    write_trace_test<true, false>(0, nt);
}

TEST_F(FileSEGYIntegWrite, FarmFileWriteRandomTraceBigNt)
{
    nt           = 3728270;
    ns           = 300;
    size_t size  = nt / 100;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy();
    write_random_trace_test<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FarmFileWriteRandomTraceWPrmBigNt)
{
    nt           = 3728270;
    ns           = 300;
    size_t size  = nt / 100;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy();
    write_random_trace_test<true, false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceZeroNt)
{
    nt = 0;
    ns = 10;
    make_segy();
    write_trace_test<false, false>(10, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    make_segy();
    write_trace_test<true, false>(10, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceZeroNt)
{
    nt           = 0;
    ns           = 10;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy();
    write_random_trace_test<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceWPrmZeroNt)
{
    nt           = 0;
    ns           = 10;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy();
    write_random_trace_test<true, false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceZeroNs)
{
    nt = 10;
    ns = 0;
    make_segy();
    write_trace_test<false, false>(10, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    make_segy();
    write_trace_test<true, false>(10, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceZeroNs)
{
    nt           = 10;
    ns           = 0;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy();
    write_random_trace_test<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceWPrmZeroNs)
{
    nt           = 10;
    ns           = 0;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_segy();
    write_random_trace_test<true, false>(size, offsets);
}
