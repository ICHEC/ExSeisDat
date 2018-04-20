#include "filesegytest.hh"

TEST_F(FileSEGYRead, FileConstructor)
{
    makeMockSEGY();

    EXPECT_EQ(piol, ReadSEGY_public::get(*file)->piol);
    EXPECT_EQ(notFile, ReadSEGY_public::get(*file)->name);
    EXPECT_EQ(mock, ReadSEGY_public::get(*file)->obj);
    ASSERT_TRUE(ns < 0x10000);
}

TEST_F(FileSEGYRead, FileReadHO)
{
    testEBCDIC = true;
    makeMockSEGY();

    EXPECT_EQ(nt, file->readNt());
    piol->isErr();

    EXPECT_EQ(ns, file->readNs());
    piol->isErr();

    const double microsecond = 1e-6;
    EXPECT_EQ(
      exseis::utils::Floating_point(inc * microsecond), file->readInc());
    piol->isErr();

    std::string text = file->readText();
    EXPECT_EQ(SEGY_utils::getTextSz(), text.size());

    // EBCDIC conversion check
    size_t slen = testString.size();
    for (size_t i = 0; i < text.size(); i++) {
        ASSERT_EQ(testString[i % slen], text[i])
          << "Loop number " << i << std::endl;
    }
}

TEST_F(FileSEGYRead, FileReadHOAPI)
{
    testEBCDIC = true;
    makeMockSEGY();

    std::string text = file->readText();
    EXPECT_EQ(3200U, text.size());
    EXPECT_EQ(SEGY_utils::getTextSz(), text.size());
    size_t slen = testString.size();
    for (size_t i = 0; i < text.size(); i++) {
        ASSERT_EQ(testString[i % slen], text[i])
          << "Loop number " << i << std::endl;
    }
}

///////////////TRACE COORDINATES + GRIDS///////////////////////////////

TEST_F(FileSEGYRead, FileReadTraceHeader)
{
    makeMockSEGY();
    initTrBlock();
    for (size_t i = 0; i < nt; i++) {
        initReadTrMock(ns, i);
    }
}

TEST_F(FileSEGYRead, FileReadTrHdrBigNs)
{
    makeMockSEGY();
    initTrBlock();

    const size_t bigns              = 10000;
    ReadSEGY_public::get(*file)->ns = bigns;
    initReadTrMock(bigns, nt / 2U);
}

TEST_F(FileSEGYRead, FileReadFileTrs)
{
    makeMockSEGY();
    initTrBlock();
    initReadTrHdrsMock(ns, nt);
}

TEST_F(FileSEGYRead, FileReadFileTrsRandom)
{
    makeMockSEGY();
    initTrBlock();
    initRandReadTrHdrsMock(ns, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigNS)
{
    nt = 100;
    ns = 10000;
    makeMockSEGY();
    readTraceTest(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigNSWPrm)
{
    nt = 100;
    ns = 10000;
    makeMockSEGY();
    initTrBlock();
    readTraceTest<true>(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigOffset)
{
    nt = 3738270;
    ns = 3000;
    makeMockSEGY();
    readTraceTest(3728270, 3000);
}

TEST_F(FileSEGYRead, FileReadTraceWPrmBigOffset)
{
    nt = 3738270;
    ns = 3000;
    makeMockSEGY();
    initTrBlock();
    readTraceTest<true>(3728270, 3000);
}

TEST_F(FileSEGYRead, FileReadRandomTrace)
{
    nt          = 3728270;
    ns          = 300;
    size_t size = 100U;
    makeMockSEGY();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest(size, offsets);
}

TEST_F(FileSEGYRead, FileReadRandomTraceWPrm)
{
    nt          = 3728270;
    ns          = 300;
    size_t size = 100U;
    makeMockSEGY();
    initTrBlock();

    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest<true>(size, offsets);
}


TEST_F(FileSEGYRead, FarmFileReadTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    makeMockSEGY();
    readTraceTest(0, nt);
}

TEST_F(FileSEGYRead, FarmFileReadTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    makeMockSEGY();
    initTrBlock();
    readTraceTest<true>(0, nt);
}

TEST_F(FileSEGYRead, FarmFileReadRandomTraceBigNt)
{
    nt          = 3728270;
    ns          = 300;
    size_t size = nt / 2;
    makeMockSEGY();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest(size, offsets);
}

TEST_F(FileSEGYRead, FarmFileReadRandomTraceWPrmBigNt)
{
    nt          = 3728270;
    ns          = 300;
    size_t size = nt / 2;
    makeMockSEGY();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSEGYRead, FileReadTraceZeroNt)
{
    nt = 0;
    ns = 10;
    makeMockSEGY();
    readTraceTest(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    makeMockSEGY();
    initTrBlock();
    readTraceTest<true>(10, nt);
}

TEST_F(FileSEGYRead, FileReadRandomTraceZeroNt)
{
    nt          = 0;
    ns          = 10;
    size_t size = nt;
    makeMockSEGY();
    initTrBlock();
    auto offsets = getRandomVec(size, 10U, 1337);
    readRandomTraceTest(size, offsets);
}

TEST_F(FileSEGYRead, FileReadRandomTraceWPrmZeroNt)
{
    nt          = 0;
    ns          = 10;
    size_t size = nt;
    makeMockSEGY();
    initTrBlock();
    auto offsets = getRandomVec(size, 10U, 1337);
    readRandomTraceTest<true>(size, offsets);
}

TEST_F(FileSEGYRead, FileReadTraceZeroNs)
{
    nt = 10;
    ns = 0;
    makeMockSEGY();
    readTraceTest(9, nt);
}

TEST_F(FileSEGYRead, FileReadTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    makeMockSEGY();
    initTrBlock();
    readTraceTest<true>(0, nt);
}

TEST_F(FileSEGYRead, FileReadRandomTraceZeroNs)
{
    nt          = 10;
    ns          = 0;
    size_t size = 5U;
    makeMockSEGY();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest(size, offsets);
}

TEST_F(FileSEGYRead, FileReadRandomTraceWPrmZeroNs)
{
    nt          = 10;
    ns          = 0;
    size_t size = 5U;
    makeMockSEGY();
    initTrBlock();
    auto offsets = getRandomVec(size, nt, 1337);
    readRandomTraceTest<true>(size, offsets);
}


TEST_F(FileSEGYRead, FileReadTraceBigNSRuleRm)
{
    nt = 100;
    ns = 10000;
    makeMockSEGY();
    readTraceTest<false, true, true>(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigNSWPrmRuleRm)
{
    nt = 100;
    ns = 10000;
    makeMockSEGY();
    initTrBlock();
    readTraceTest<true, true, true>(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigOffsetRuleRm)
{
    nt = 3738270;
    ns = 3000;
    makeMockSEGY();
    readTraceTest<false, true, true>(3728270, 3000);
}

TEST_F(FileSEGYRead, FileReadTraceWPrmBigOffsetRuleRm)
{
    nt = 3738270;
    ns = 3000;
    makeMockSEGY();
    initTrBlock();
    readTraceTest<true, true, true>(3728270, 3000);
}
