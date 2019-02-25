#include "filesegytest.hh"

// TODO: Add tests were trace header writes cause a set_file_size call

TEST_F(FileSEGYWrite, FileWriteHO)
{
    make_mock_segy<true>();
}

TEST_F(FileSEGYWrite, FileWriteHOLongString)
{
    // Extend the string beyond the text boundary
    // Extended text should be dropped in write call
    const size_t sz        = test_string.size();
    const size_t extend_sz = 3400U - sz;
    test_string.resize(sz + extend_sz);
    for (size_t i = 3200U; i < sz + extend_sz; i++) {
        test_string[i] = 0x7F;
    }

    make_mock_segy<true>();
}

TEST_F(FileSEGYWrite, FileWriteHOEmptyString)
{
    test_string.resize(0);
    make_mock_segy<true>();
}

TEST_F(FileSEGYWrite, FileWriteTrHdrGrid)
{
    make_mock_segy<true>();
    for (size_t i = 0; i < nt; i++) {
        write_tr_hdr_grid_test(i);
    }
}

TEST_F(FileSEGYWrite, FileWriteTrHdrCoord1)
{
    make_mock_segy<true>();
    std::vector<unsigned char> tr(segy::segy_trace_header_size());
    init_write_tr_hdr_coord({xCMP, yCMP}, {160010, 240022}, -100, 10U, &tr);


    Trace_metadata prm(1U);
    prm.set_floating_point(0, Meta::xCmp, 1600.1);
    prm.set_floating_point(0, Meta::yCmp, 2400.22);
    prm.set_integer(0, Meta::tn, 10U);
    file->write_param(10U, 1U, &prm);
}

TEST_F(FileSEGYWrite, FileWriteTrHdrCoord2)
{
    make_mock_segy<true>();
    std::vector<unsigned char> tr(segy::segy_trace_header_size());
    init_write_tr_hdr_coord(
        {x_src, y_src}, {1600100, 3400222}, -1000, 10U, &tr);

    Trace_metadata prm(1U);
    prm.set_floating_point(0, Meta::x_src, 1600.1000);
    prm.set_floating_point(0, Meta::y_src, 3400.2220);
    prm.set_integer(0, Meta::tn, 10U);

    file->write_param(10U, 1U, &prm);
}

TEST_F(FileSEGYWrite, FileWriteTrHdrCoord3)
{
    make_mock_segy<true>();
    std::vector<unsigned char> tr(segy::segy_trace_header_size());
    init_write_tr_hdr_coord(
        {x_src, y_src}, {1623001001, 34002220}, -10000, 10U, &tr);

    Trace_metadata prm(1U);
    prm.set_floating_point(0, Meta::x_src, 162300.10009);
    prm.set_floating_point(0, Meta::y_src, 3400.22201);
    prm.set_integer(0, Meta::tn, 10U);
    file->write_param(10U, 1U, &prm);
}

TEST_F(FileSEGYWrite, FileWriteTraceNormal)
{
    nt = 100;
    ns = 300;
    make_mock_segy<false>();
    write_trace_test(0U, nt);
}

TEST_F(FileSEGYWrite, FileWriteTraceWPrmNormal)
{
    nt = 100;
    ns = 300;
    make_mock_segy<false>();
    write_trace_test<true>(0U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceNormal)
{
    nt           = 100;
    ns           = 300;
    size_t size  = 1U;
    auto offsets = get_random_vec(size, nt, 1337);
    make_mock_segy<false>();
    write_random_trace_test(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceWPrmNormal)
{
    nt           = 100;
    ns           = 300;
    size_t size  = 1U;
    auto offsets = get_random_vec(size, nt, 1337);
    make_mock_segy<false>();
    write_random_trace_test<true>(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceBigNs)
{
    nt = 100;
    ns = 10000;
    make_mock_segy<false>();
    write_trace_test(10U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceBigNs)
{
    nt           = 100;
    ns           = 300;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_mock_segy<false>();
    write_random_trace_test(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceWPrmBigNs)
{
    nt = 100;
    ns = 10000;
    make_mock_segy<false>();
    write_trace_test<true>(10U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceWPrmBigNs)
{
    nt           = 100;
    ns           = 300;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_mock_segy<false>();
    write_random_trace_test<true>(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceBigOffset)
{
    nt = 3000;
    ns = 3000;
    make_mock_segy<false>();
    write_trace_test(3728270U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceBigOffset)
{
    nt           = 3000;
    ns           = 3000;
    size_t size  = 1U;
    auto offsets = std::vector<size_t>(size);
    offsets[0]   = 3001;
    make_mock_segy<false>();
    write_random_trace_test(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceWPrmBigOffset)
{
    nt = 3000;
    ns = 3000;
    make_mock_segy<false>();
    write_trace_test<true>(3728270U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceWPrmBigOffset)
{
    nt           = 3000;
    ns           = 3000;
    size_t size  = 1U;
    auto offsets = std::vector<size_t>(size);
    offsets[0]   = 3001;
    make_mock_segy<false>();
    write_random_trace_test<true>(size, offsets);
}

TEST_F(FileSEGYWrite, FarmFileWriteTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    make_mock_segy<false>();
    write_trace_test(0U, nt);
}

TEST_F(FileSEGYWrite, FarmFileWriteRandomTraceBigNt)
{
    nt           = 3728270;
    ns           = 300;
    size_t size  = nt / 2;
    auto offsets = get_random_vec(size, nt, 1337);
    make_mock_segy<false>();
    write_random_trace_test(size, offsets);
}

TEST_F(FileSEGYWrite, FarmFileWriteTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    make_mock_segy<false>();
    write_trace_test<true>(0U, nt);
}

TEST_F(FileSEGYWrite, FarmFileWriteRandomTraceWPrmBigNt)
{
    nt           = 3728270;
    ns           = 300;
    size_t size  = nt / 2;
    auto offsets = get_random_vec(size, nt, 1337);
    make_mock_segy<false>();
    write_random_trace_test<true>(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceZeroNt)
{
    nt = 0;
    ns = 10;
    make_mock_segy<false>();
    write_trace_test(10U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceZeroNt)
{
    nt           = 0;
    ns           = 10;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_mock_segy<false>();
    write_random_trace_test(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    make_mock_segy<false>();
    write_trace_test<true>(10U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceWPrmZeroNt)
{
    nt           = 0;
    ns           = 10;
    size_t size  = nt;
    auto offsets = get_random_vec(size, nt, 1337);
    make_mock_segy<false>();
    write_random_trace_test<true>(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceZeroNs)
{
    nt = 10;
    ns = 0;
    make_mock_segy<false>();
    write_trace_test(10U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceZeroNs)
{
    nt           = 10;
    ns           = 0;
    size_t size  = 10U;
    auto offsets = get_random_vec(size, nt, 1337);
    make_mock_segy<false>();
    write_random_trace_test(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    make_mock_segy<false>();
    write_trace_test<true>(10U, nt);
}

TEST_F(FileSEGYWrite, FileWriteRandomTraceWPrmZeroNs)
{
    nt           = 10;
    ns           = 0;
    size_t size  = 10U;
    auto offsets = get_random_vec(size, nt, 1337);
    make_mock_segy<false>();
    write_random_trace_test<true>(size, offsets);
}

TEST_F(FileSEGYWrite, FileWriteTrace_metadata)
{
    make_mock_segy<true>();
    write_trace_header_test<false>(0U, nt);
}

TEST_F(FileSEGYWrite, FileWriteTrace_metadataOne)
{
    nt = 400;
    make_mock_segy<true>();
    write_trace_header_test<false>(200, 1);
}

TEST_F(FileSEGYWrite, FileWriteTrace_metadataBigOffset)
{
    nt = 10000000U;
    make_mock_segy<true>();
    write_trace_header_test<false>(nt - 1, 1);
}

TEST_F(FileSEGYWrite, FileWriteTrace_metadataCopy)
{
    make_mock_segy<true>();
    write_trace_header_test<true>(0U, nt);
}

TEST_F(FileSEGYWrite, FileWriteTrace_metadataOneCopy)
{
    nt = 400;
    make_mock_segy<true>();
    write_trace_header_test<true>(200, 1);
}

TEST_F(FileSEGYWrite, FileWriteTrace_metadataBigOffsetCopy)
{
    nt = 10000000U;
    make_mock_segy<true>();
    write_trace_header_test<true>(nt - 1, 1);
}

// // Akward to fit this into the current functions
// TEST_F(FileSEGYDeath, FileWriteTraceTrace_metadataBigOffset)
// {
//     make_mock_segy<true>();
//     write_trace_header_test<false, false>(NT_LIMITS + 1);
//     EXPECT_EXIT(
//       piol->assert_ok(), ExitedWithCode(EXIT_FAILURE),
//       ".*Fatal Error in PIOL\\. Dumping Log\\..*");
// }

////////////////////////////////// DEATH TESTS /////////////////////////////////
typedef FileSEGYWrite FileSEGYDeath;

TEST_F(FileSEGYDeath, FileWriteAPIBadns)
{
    ns = 0x470000;
    make_mock_segy<false>();
    file->write_ns(ns);
    mock_object.reset();
    EXPECT_EXIT(
        piol->assert_ok(), ExitedWithCode(EXIT_FAILURE),
        ".*Fatal Error in PIOL\\. Dumping Log\\..*");
}

#ifdef NT_LIMITS
TEST_F(FileSEGYDeath, FileWriteAPIBadnt)
{
    nt = NT_LIMITS + 1;
    make_mock_segy<false>();
    file->write_nt(nt);

    mock_object.reset();
    EXPECT_EXIT(
        piol->assert_ok(), ExitedWithCode(EXIT_FAILURE),
        ".*Fatal Error in PIOL\\. Dumping Log\\..*");
}
#endif

TEST_F(FileSEGYDeath, FileWriteAPIBadSampleInterval)
{
    exseis::utils::Floating_point gsample_interval = nan("");
    make_mock_segy<false>();
    file->write_sample_interval(gsample_interval);

    mock_object.reset();
    EXPECT_EXIT(
        piol->assert_ok(), ExitedWithCode(EXIT_FAILURE),
        ".*Fatal Error in PIOL\\. Dumping Log\\..*");
}
