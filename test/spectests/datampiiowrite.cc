#include <sys/stat.h>
#include "datampiiotest.hh"
TEST_F(MPIIOTest, SetZeroFileSz)
{
    makeTestSz(0);
}

//Takes about 3 seconds on Fionn
TEST_F(MPIIOTest, FarmSetNormalFileSz)
{
    makeTestSz(2*prefix(3));
}

TEST_F(MPIIOTest, FarmSetLargeFileSz)
{
    makeTestSz(10*prefix(3));
}

TEST_F(MPIIOTest, WriteContigZero)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 0;
    const size_t ns = 0;
    writeSmallBlocks<false>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, WriteContigSSS)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 400;
    const size_t ns = 261;
    writeSmallBlocks<false>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, WriteContigSLS)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 40U;
    const size_t ns = 40000U;
    writeSmallBlocks<false>(nt, ns, 1000U);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmWriteContigSLM)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 40000U;
    const size_t ns = 40000U;
    writeSmallBlocks<false>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmWriteContigSLL)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    writeSmallBlocks<false>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmWriteContigLLM)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 40000U;
    const size_t ns = 40000U;
    writeBigBlocks<false>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmWriteContigMLL)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    writeBigBlocks<false>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, WriteBlocksZero)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 0;
    const size_t ns = 0;
    writeSmallBlocks<true>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, WriteBlocksSSS)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 400;
    const size_t ns = 261;
    writeSmallBlocks<true>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, WriteBlocksSLS)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 40U;
    const size_t ns = 40000U;
    writeSmallBlocks<true>(nt, ns, 1000U);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmWriteBlocksSLM)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 40000U;
    const size_t ns = 40000U;
    writeSmallBlocks<true>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmWriteBlocksSLL)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    writeSmallBlocks<true>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmWriteBlocksLLM)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 40000U;
    const size_t ns = 40000U;
    writeBigBlocks<true>(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmWriteBlocksMLL)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    writeBigBlocks<true>(nt, ns);
    piol->isErr();
}

////////Lists///////
TEST_F(MPIIOTest, WriteListZero)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 0;
    const size_t ns = 0;
    writeList(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, WriteListSmall)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 400;
    const size_t ns = 261;

    writeList(nt, ns);
    piol->isErr();
}

TEST_F(MPIIOTest, FarmWriteListLarge)
{
    makeMPIIO<true>(tempFile);
    const size_t nt = 2000000U;
    const size_t ns = 1000U;
    writeList(nt, ns);
    piol->isErr();
}

