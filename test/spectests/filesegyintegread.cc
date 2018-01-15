#include "filesegytest.hh"

const size_t largens = 1000U;
const size_t largent = 2000000U;
const size_t bigtns = 32000U;
const size_t smallns = 261U;
const size_t smallnt = 400U;

//Read test of File::SEGY -> Obj::SEGY -> Data::MPIIO
TEST_F(FileSEGYIntegRead, SEGYReadHO)
{
    nt = smallnt;
    ns = smallns;
    makeSEGY<false>(smallSEGYFile);

    piol->isErr();
    EXPECT_EQ(ns, file->readNs());
    piol->isErr();
    EXPECT_EQ(nt, file->readNt());
    piol->isErr();
    if (sizeof(geom_t) == sizeof(double))
        EXPECT_DOUBLE_EQ(double(20e-6), file->readInc());
    else
        EXPECT_FLOAT_EQ(float(20e-6), file->readInc());
}

TEST_F(FileSEGYIntegRead, FileReadTraceSmall)
{
    nt = smallnt;
    ns = smallns;
    makeSEGY<false>(smallSEGYFile);
    readTraceTest<false, false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FileReadTraceWPrmSmall)
{
    nt = smallnt;
    ns = smallns;
    makeSEGY<false>(smallSEGYFile);
    readTraceTest<true, false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FileReadRandomTraceSmall)
{
    nt = smallnt;
    ns = smallns;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<false>(smallSEGYFile);
    readRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FileReadRandomTraceWPrmSmall)
{
    nt = smallnt;
    ns = smallns;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<false>(smallSEGYFile);
    readRandomTraceTest<true, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FileReadTraceSmallOpts)
{
    nt = smallnt;
    ns = smallns;
    makeSEGY<true>(smallSEGYFile);
    readTraceTest<false, false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FileReadTraceWPrmSmallOpts)
{
    nt = smallnt;
    ns = smallns;
    makeSEGY<true>(smallSEGYFile);
    readTraceTest<true, false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FileReadRandomTraceSmallOpts)
{
    nt = smallnt;
    ns = smallns;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<false>(smallSEGYFile);
    readRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FileReadRandomTraceWPrmSmallOpts)
{
    nt = smallnt;
    ns = smallns;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<false>(smallSEGYFile);
    readRandomTraceTest<true, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FileReadTraceBigNS)
{
    nt = 200;
    ns = bigtns;
    makeSEGY<false>(bigTraceSEGYFile);
    readTraceTest<false, false>(10, nt);
}

TEST_F(FileSEGYIntegRead, FileReadTraceWPrmBigNS)
{
    nt = 200;
    ns = bigtns;
    makeSEGY<false>(bigTraceSEGYFile);
    readTraceTest<true, false>(10, nt);
}

TEST_F(FileSEGYIntegRead, FileReadRandomTraceBigNS)
{
    nt = 200;
    ns = bigtns;
    size_t size = 2;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<false>(bigTraceSEGYFile);
    readRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FileReadRandomTraceWPrmBigNS)
{
    nt = 200;
    ns = bigtns;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<false>(bigTraceSEGYFile);
    readRandomTraceTest<true, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FileReadTraceBigOffset)
{
    nt = 10;
    ns = largens;
    makeSEGY<false>(largeSEGYFile);
    readTraceTest<false, false>(1999990U, nt);
}

TEST_F(FileSEGYIntegRead, FileReadTraceWPrmBigOffset)
{
    nt = 10;
    ns = largens;
    makeSEGY<false>(largeSEGYFile);
    readTraceTest<true, false>(1999990U, nt);
}

TEST_F(FileSEGYIntegRead, FarmFileReadTraceBigNt)
{
    nt = largent;
    ns = largens;
    makeSEGY<false>(largeSEGYFile);
    readTraceTest<false, false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FarmFileReadTraceWPrmBigNt)
{
    nt = largent;
    ns = largens;
    makeSEGY<false>(largeSEGYFile);
    readTraceTest<true, false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FarmFileReadRandomTraceBigNt)
{
    nt = largent;
    ns = largens;
    size_t size = nt/2;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<false>(largeSEGYFile);
    readRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FarmFileReadRandomTraceWPrmBigNt)
{
    nt = largent;
    ns = largens;
    size_t size = nt/2;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<false>(largeSEGYFile);
    readRandomTraceTest<true, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FileReadTraceZeroNt)
{
    nt = 0U;
    ns = largens;
    makeSEGY<false>(largeSEGYFile);
    readTraceTest<false, false>(10, nt);
}

TEST_F(FileSEGYIntegRead, FileReadTraceWPrmZeroNt)
{
    nt = 0U;
    ns = largens;
    makeSEGY<false>(largeSEGYFile);
    readTraceTest<true, false>(10, nt);
}

TEST_F(FileSEGYIntegRead, FarmFileReadRandomTraceZeroNt)
{
    nt = 0U;
    ns = largens;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<false>(largeSEGYFile);
    readRandomTraceTest<false, false>(size, offsets);
}

TEST_F(FileSEGYIntegRead, FarmFileReadRandomTraceWPrmZeroNt)
{
    nt = 0U;
    ns = largens;
    size_t size = nt;
    auto offsets = getRandomVec(size, nt, 1337);
    makeSEGY<false>(largeSEGYFile);
    readRandomTraceTest<true, false>(size, offsets);
}

