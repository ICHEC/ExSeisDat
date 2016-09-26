#include "filesegytest.hh"

csize_t largens = 1000U;
csize_t largent = 2000000U;
csize_t bigtns = 32000U;
csize_t bigtnt = 40000U;
csize_t smallns = 261U;
csize_t smallnt = 400U;

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
    EXPECT_DOUBLE_EQ(double(20e-6), file->readInc());
}

TEST_F(FileSEGYIntegRead, FileReadTraceSmall)
{
    nt = smallnt;
    ns = smallns;
    makeSEGY<false>(smallSEGYFile);
    readTraceTest<false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FileReadTraceSmallOpts)
{
    nt = smallnt;
    ns = smallns;
    makeSEGY<false, true>(smallSEGYFile);
    readTraceTest<false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FileReadTraceBigNS)
{
    nt = 200;
    ns = bigtns;
    makeSEGY<false>(bigTraceSEGYFile);
    readTraceTest<false>(10, nt);
}

TEST_F(FileSEGYIntegRead, FileReadTraceBigOffset)
{
    nt = 10;
    ns = largens;
    makeSEGY<false>(largeSEGYFile);
    readTraceTest<false>(1999990U, nt);
}

TEST_F(FileSEGYIntegRead, FarmFileReadTraceBigNt)
{
    nt = largent;
    ns = largens;
    makeSEGY<false>(largeSEGYFile);
    readTraceTest<false>(0, nt);
}

TEST_F(FileSEGYIntegRead, FileReadTraceZeroNt)
{
    nt = 0U;
    ns = largens;
    makeSEGY<false>(largeSEGYFile);
    readTraceTest<false>(10, nt);
}

