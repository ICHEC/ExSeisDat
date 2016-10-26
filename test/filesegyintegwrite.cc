#include "filesegytest.hh"

//Write test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileSEGYIntegWrite, SEGYWriteReadHO)
{
    ns = 261U;
    nt = 400U;

    makeSEGY<true>(tempFile);
    EXPECT_EQ(ns, file->readNs());
    piol->isErr();

    EXPECT_EQ(nt, file->readNt());
    piol->isErr();

    std::string text = file->readText();
    piol->isErr();
    ASSERT_TRUE(testString.size() <= text.size());
    for (size_t i = 0; i < testString.size(); i++)
        ASSERT_EQ(testString[i], text[i]);
}

//Write test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileSEGYIntegWrite, SEGYWriteReadTraceParam)
{
    ns = 261U;
    nt = 400U;
    File::coord_t coord(1600, 2000);
    File::grid_t grid(ilNum(201), xlNum(201));
    TraceParam prm, prm2;

    makeSEGY<true>(tempFile);

    piol->isErr();
    file->file->ns = ns;
    file->writeNt(nt);
    piol->isErr();

    prm.line = grid;
    prm.cmp = coord;
    file->writeTraceParam(201U, 1U, &prm);
    file->readTraceParam(201U, 1U, &prm);

    EXPECT_EQ(grid.il, prm.line.il);
    EXPECT_EQ(grid.xl, prm.line.xl);
    EXPECT_EQ(coord.x, prm.cmp.x);
    EXPECT_EQ(coord.y, prm.cmp.y);
    //TODO: Add the rest
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceNormal)
{
    nt = 100;
    ns = 300;
    makeSEGY<true>(tempFile);
    writeTraceTest<false,false>(0, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceWPrmNormal)
{
    nt = 100;
    ns = 300;
    makeSEGY<true>(tempFile);
    writeTraceTest<true,false>(0, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceNormal)
{
    nt = 100;
    ns = 300;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<true>(tempFile);
    writeRandomTraceTest<false,false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceWPrmNormal)
{
    nt = 100;
    ns = 300;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<true>(tempFile);
    writeRandomTraceTest<true,false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceNormalOpt)
{
    nt = 100;
    ns = 300;
    makeSEGY<true, true>(tempFile);
    writeTraceTest<false,false>(0, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceWPrmNormalOpt)
{
    nt = 100;
    ns = 300;
    makeSEGY<true, true>(tempFile);
    writeTraceTest<true,false>(0, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceNormalOpt)
{
    nt = 100;
    ns = 300;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<true, true>(tempFile);
    writeRandomTraceTest<false,false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceWPrmNormalOpt)
{
    nt = 100;
    ns = 300;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<true, true>(tempFile);
    writeRandomTraceTest<true,false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceBigNs)
{
    nt = 100;
    ns = 10000;
    makeSEGY<true>(tempFile);
    writeTraceTest<false,false>(10, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceWPrmBigNs)
{
    nt = 100;
    ns = 10000;
    makeSEGY<true>(tempFile);
    writeTraceTest<true,false>(10, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceBigNs)
{
    nt = 100;
    ns = 10000;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<true>(tempFile);
    writeRandomTraceTest<false,false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceWPrmBigNs)
{
    nt = 100;
    ns = 10000;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<true>(tempFile);
    writeRandomTraceTest<true,false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceBigOffset)
{
    nt = 10;
    ns = 3000;
    makeSEGY<true>(tempFile);
    writeTraceTest<false,false>(3728270, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceWPrmBigOffset)
{
    nt = 10;
    ns = 3000;
    makeSEGY<true>(tempFile);
    writeTraceTest<true,false>(3728270, nt);
}

TEST_F(FileSEGYIntegWrite, FarmFileWriteTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    makeSEGY<true>(tempFile);
    writeTraceTest<false,false>(0, nt);
}

TEST_F(FileSEGYIntegWrite, FarmFileWriteTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    makeSEGY<true>(tempFile);
    writeTraceTest<true,false>(0, nt);
}

TEST_F(FileSEGYIntegWrite, FarmFileWriteRandomTraceBigNt)
{
    nt = 3728270;
    ns = 300;
    size_t size = nt/100;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<true>(tempFile);
    writeRandomTraceTest<false,false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FarmFileWriteRandomTraceWPrmBigNt)
{
    nt = 3728270;
    ns = 300;
    size_t size = nt/100;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<true>(tempFile);
    writeRandomTraceTest<true,false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceZeroNt)
{
    nt = 0;
    ns = 10;
    makeSEGY<true>(tempFile);
    writeTraceTest<false,false>(10, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    makeSEGY<true>(tempFile);
    writeTraceTest<true,false>(10, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceZeroNt)
{
    nt = 0;
    ns = 10;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<true>(tempFile);
    writeRandomTraceTest<false,false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceWPrmZeroNt)
{
    nt = 0;
    ns = 10;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<true>(tempFile);
    writeRandomTraceTest<true,false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceZeroNs)
{
    nt = 10;
    ns = 0;
    makeSEGY<true>(tempFile);
    writeTraceTest<false,false>(10, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    makeSEGY<true>(tempFile);
    writeTraceTest<true,false>(10, nt);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceZeroNs)
{
    nt = 10;
    ns = 0;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<true>(tempFile);
    writeRandomTraceTest<false,false>(size, offsets);
}

TEST_F(FileSEGYIntegWrite, FileWriteRandomTraceWPrmZeroNs)
{
    nt = 10;
    ns = 0;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<true>(tempFile);
    writeRandomTraceTest<true,false>(size, offsets);
}
