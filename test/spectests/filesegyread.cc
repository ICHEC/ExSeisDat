#include "filesegytest.hh"

using FileSEGYReadDeathTest = FileSEGYRead;
TEST_F(FileSEGYReadDeathTest, BadNameConstructor)
{
    ReadSEGY file(piol, nonexistant_filename());
    EXPECT_EXIT(
      piol->assert_ok(), ExitedWithCode(EXIT_FAILURE),
      ".*Fatal Error in PIOL\\. Dumping Log\\..*");
}

TEST_F(FileSEGYRead, FileConstructor)
{
    auto filename = make_mock_segy();

    EXPECT_EQ(piol, ReadSEGY_public::get(file.get())->m_piol);
    EXPECT_EQ(filename, ReadSEGY_public::get(file.get())->m_name);
    EXPECT_EQ(mock_object, ReadSEGY_public::get(file.get())->m_obj);
    ASSERT_TRUE(ns < 0x10000);
}

TEST_F(FileSEGYRead, FileReadHO)
{
    test_ebcdic = true;
    make_mock_segy();

    EXPECT_EQ(nt, file->read_nt());
    piol->assert_ok();

    EXPECT_EQ(ns, file->read_ns());
    piol->assert_ok();

    const double microsecond = 1e-6;
    EXPECT_EQ(
      exseis::utils::Floating_point(sample_interval * microsecond),
      file->read_sample_interval());
    piol->assert_ok();

    std::string text = file->read_text();
    EXPECT_EQ(segy::segy_text_header_size(), text.size());

    // EBCDIC conversion check
    size_t slen = test_string.size();
    for (size_t i = 0; i < text.size(); i++) {
        ASSERT_EQ(test_string[i % slen], text[i])
          << "Loop number " << i << std::endl;
    }
}

TEST_F(FileSEGYRead, FileReadHOAPI)
{
    test_ebcdic = true;
    make_mock_segy();

    std::string text = file->read_text();
    EXPECT_EQ(3200U, text.size());
    EXPECT_EQ(segy::segy_text_header_size(), text.size());
    size_t slen = test_string.size();
    for (size_t i = 0; i < text.size(); i++) {
        ASSERT_EQ(test_string[i % slen], text[i])
          << "Loop number " << i << std::endl;
    }
}

///////////////TRACE COORDINATES + GRIDS///////////////////////////////

TEST_F(FileSEGYRead, FileReadTraceHeader)
{
    make_mock_segy();
    init_tr_block();
    for (size_t i = 0; i < nt; i++) {
        init_read_tr_mock(ns, i);
    }
}

TEST_F(FileSEGYRead, FileReadTrHdrBigNs)
{
    make_mock_segy();
    init_tr_block();

    const size_t bigns                     = 10000;
    ReadSEGY_public::get(file.get())->m_ns = bigns;
    init_read_tr_mock(bigns, nt / 2U);
}

TEST_F(FileSEGYRead, FileReadFileTrs)
{
    make_mock_segy();
    init_tr_block();
    init_read_tr_hdrs_mock(ns, nt);
}

TEST_F(FileSEGYRead, FileReadFileTrsRandom)
{
    make_mock_segy();
    init_tr_block();
    init_rand_read_tr_hdrs_mock(ns, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigNS)
{
    nt = 100;
    ns = 10000;
    make_mock_segy();
    read_trace_test(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigNSWPrm)
{
    nt = 100;
    ns = 10000;
    make_mock_segy();
    init_tr_block();
    read_trace_test<true>(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigOffset)
{
    nt = 3738270;
    ns = 3000;
    make_mock_segy();
    read_trace_test(3728270, 3000);
}

TEST_F(FileSEGYRead, FarmFileReadTraceWPrmBigOffset)
{
    nt = 3738270;
    ns = 3000;
    make_mock_segy();
    init_tr_block();
    read_trace_test<true>(3728270, 3000);
}

TEST_F(FileSEGYRead, FileReadRandomTrace)
{
    nt          = 3728270;
    ns          = 300;
    size_t size = 100U;
    make_mock_segy();
    init_tr_block();
    auto offsets = get_random_vec(size, nt, 1337);
    read_random_trace_test(size, offsets);
}

TEST_F(FileSEGYRead, FileReadRandomTraceWPrm)
{
    nt          = 3728270;
    ns          = 300;
    size_t size = 100U;
    make_mock_segy();
    init_tr_block();

    auto offsets = get_random_vec(size, nt, 1337);
    read_random_trace_test<true>(size, offsets);
}


TEST_F(FileSEGYRead, FarmFileReadTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    make_mock_segy();
    read_trace_test(0, nt);
}

TEST_F(FileSEGYRead, FarmFileReadTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    make_mock_segy();
    init_tr_block();
    read_trace_test<true>(0, nt);
}

TEST_F(FileSEGYRead, FarmFileReadRandomTraceBigNt)
{
    nt          = 3728270;
    ns          = 300;
    size_t size = nt / 2;
    make_mock_segy();
    init_tr_block();
    auto offsets = get_random_vec(size, nt, 1337);
    read_random_trace_test(size, offsets);
}

TEST_F(FileSEGYRead, FarmFileReadRandomTraceWPrmBigNt)
{
    nt          = 3728270;
    ns          = 300;
    size_t size = nt / 2;
    make_mock_segy();
    init_tr_block();
    auto offsets = get_random_vec(size, nt, 1337);
    read_random_trace_test<true>(size, offsets);
}

TEST_F(FileSEGYRead, FileReadTraceZeroNt)
{
    nt = 0;
    ns = 10;
    make_mock_segy();
    read_trace_test(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    make_mock_segy();
    init_tr_block();
    read_trace_test<true>(10, nt);
}

TEST_F(FileSEGYRead, FileReadRandomTraceZeroNt)
{
    nt          = 0;
    ns          = 10;
    size_t size = nt;
    make_mock_segy();
    init_tr_block();
    auto offsets = get_random_vec(size, 10U, 1337);
    read_random_trace_test(size, offsets);
}

TEST_F(FileSEGYRead, FileReadRandomTraceWPrmZeroNt)
{
    nt          = 0;
    ns          = 10;
    size_t size = nt;
    make_mock_segy();
    init_tr_block();
    auto offsets = get_random_vec(size, 10U, 1337);
    read_random_trace_test<true>(size, offsets);
}

TEST_F(FileSEGYRead, FileReadTraceZeroNs)
{
    nt = 10;
    ns = 0;
    make_mock_segy();
    read_trace_test(9, nt);
}

TEST_F(FileSEGYRead, FileReadTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    make_mock_segy();
    init_tr_block();
    read_trace_test<true>(0, nt);
}

TEST_F(FileSEGYRead, FileReadRandomTraceZeroNs)
{
    nt          = 10;
    ns          = 0;
    size_t size = 5U;
    make_mock_segy();
    init_tr_block();
    auto offsets = get_random_vec(size, nt, 1337);
    read_random_trace_test(size, offsets);
}

TEST_F(FileSEGYRead, FileReadRandomTraceWPrmZeroNs)
{
    nt          = 10;
    ns          = 0;
    size_t size = 5U;
    make_mock_segy();
    init_tr_block();
    auto offsets = get_random_vec(size, nt, 1337);
    read_random_trace_test<true>(size, offsets);
}


TEST_F(FileSEGYRead, FileReadTraceBigNSRuleRm)
{
    nt = 100;
    ns = 10000;
    make_mock_segy();
    read_trace_test<false, true, true>(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigNSWPrmRuleRm)
{
    nt = 100;
    ns = 10000;
    make_mock_segy();
    init_tr_block();
    read_trace_test<true, true, true>(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigOffsetRuleRm)
{
    nt = 3738270;
    ns = 3000;
    make_mock_segy();
    read_trace_test<false, true, true>(3728270, 3000);
}

TEST_F(FileSEGYRead, FarmFileReadTraceWPrmBigOffsetRuleRm)
{
    nt = 3738270;
    ns = 3000;
    make_mock_segy();
    init_tr_block();
    read_trace_test<true, true, true>(3728270, 3000);
}
