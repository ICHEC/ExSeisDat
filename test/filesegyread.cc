#include "filesegytest.hh"

TEST_F(FileSEGYRead, FileConstructor)
{
    makeMockSEGY<false>();
    EXPECT_EQ(piol, file->piol);
    EXPECT_EQ(notFile, file->name);
    EXPECT_EQ(mock, file->obj);
    ASSERT_TRUE(ns < 0x10000);
}

TEST_F(FileSEGYRead, FileReadHO)
{
    makeMockSEGY<false>();

    EXPECT_EQ(nt, file->readNt());
    piol->isErr();

    EXPECT_EQ(ns, file->readNs());
    piol->isErr();

    EXPECT_EQ(geom_t(inc*SI::Micro), file->readInc());
    piol->isErr();

//    EXPECT_EQ(format, static_cast<size_t>(file->format));
//    piol->isErr();

    std::string text = file->readText();
    EXPECT_EQ(SEGSz::getTextSz(), text.size());

//EBCDIC conversion check
    size_t slen = testString.size();
    for (size_t i = 0; i < text.size(); i++)
        ASSERT_EQ(testString[i % slen], text[i]) << "Loop number " << i << std::endl;
}

TEST_F(FileSEGYRead, FileReadHOAPI)
{
    testEBCDIC = true;
    makeMockSEGY<false>();

    std::string text = file->readText();
    EXPECT_EQ(3200U, text.size());
    EXPECT_EQ(SEGSz::getTextSz(), text.size());
    size_t slen = testString.size();
    for (size_t i = 0; i < text.size(); i++)
        ASSERT_EQ(testString[i % slen], text[i]) << "Loop number " << i << std::endl;
}

///////////////TRACE COORDINATES + GRIDS///////////////////////////////

TEST_F(FileSEGYRead, FileReadTraceHeader)
{
    makeMockSEGY<false>();
    initTrBlock();
    for (size_t i = 0; i < nt; i++)
        initReadTrMock(ns, i);
}

TEST_F(FileSEGYRead, FileReadTrHdrBigNs)
{
    makeMockSEGY<false>();
    initTrBlock();

    const size_t bigns = 10000;
    file->ns = bigns;
    initReadTrMock(bigns, nt/2U);
}

TEST_F(FileSEGYRead, FileReadFileTrs)
{
    makeMockSEGY<false>();
    initTrBlock();
    initReadTrHdrsMock(ns, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigNS)
{
    nt = 100;
    ns = 10000;
    makeMockSEGY<false>();
    readTraceTest(10, nt);
    readTraceWPrmTest(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceBigOffset)
{
    nt = 3738270;
    ns = 3000;
    makeMockSEGY<false>();
#warning //TODO: should be readTraceTest( 3728270, nt)?
    readTraceTest(3728270, 3000);
    readTraceWPrmTest(3728270, 3000);
}

TEST_F(FileSEGYRead, FarmFileReadTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    makeMockSEGY<false>();
    readTraceTest(0, nt);
    readTraceWPrmTest(0, nt);
}

TEST_F(FileSEGYRead, FileReadTraceZeroNt)
{
    nt = 0;
    ns = 10;
    makeMockSEGY<false, false>();
    readTraceTest(10, nt);
    readTraceWPrmTest(10, nt);
}

TEST_F(FileSEGYRead, FileReadTraceZeroNs)
{
    nt = 10;
    ns = 0;
    makeMockSEGY<false, false>();
    readTraceTest(10, nt);
    readTraceWPrmTest(10, nt);
}
