#include "objsegytest.hh"
TEST_F(ObjIntegTest, SEGYHOWrite1)
{
    makeRealSEGY<true>(tempFile);
    writeHOPattern<false>(0, 107);
}

TEST_F(ObjIntegTest, SEGYHOWrite2)
{
    makeRealSEGY<true>(tempFile);
    writeHOPattern<false>(0, 46);
}

TEST_F(ObjIntegTest, SEGYHOWrite3)
{
    makeRealSEGY<true>(tempFile);
    writeHOPattern<false>(0, 0);
}

TEST_F(ObjIntegTest, SEGYWriteSingle1)
{
    makeRealSEGY<true>(tempFile);
    writeTest<Block::DOMD, false>(10U, 1U, 200, 0, 117);
    writeTest<Block::DODF, false>(10U, 1U, 200, 0, 117);
    writeTest<Block::DO, false>(10U, 1U, 200, 0, 117);
}

TEST_F(ObjIntegTest, SEGYWriteSingle2)
{
    makeRealSEGY<true>(tempFile);
    writeTest<Block::DOMD, false>(10U, 1U, 200, 0, 13);
    writeTest<Block::DODF, false>(10U, 1U, 200, 0, 13);
    writeTest<Block::DO, false>(10U, 1U, 200, 0, 13);
}

TEST_F(ObjIntegTest, SEGYWriteZeroNt)
{
    makeRealSEGY<true>(tempFile);
    writeTest<Block::DOMD, false>(10U, 0U, 2000);
    writeTest<Block::DODF, false>(10U, 0U, 200);
    writeTest<Block::DO, false>(10U, 0U, 200);
}

TEST_F(ObjIntegTest, SEGYWriteZeroNs)
{
    makeRealSEGY<true>(tempFile);
    writeTest<Block::DOMD, false>(10U, 100U, 0U);
    writeTest<Block::DODF, false>(10U, 100U, 0U);
    writeTest<Block::DO, false>(10U, 100U, 0U);
}

TEST_F(ObjIntegTest, SEGYWrite)
{
    makeRealSEGY<true>(tempFile);
    writeTest<Block::DOMD, false>(10U, 100U, 2000);
    writeTest<Block::DODF, false>(10U, 100U, 2000);
    writeTest<Block::DO, false>(10U, 100U, 2000);
}

TEST_F(ObjIntegTest, FarmSEGYBigWrite)
{
    makeRealSEGY<true>(tempFile);
    writeTest<Block::DOMD, false>(10U, 300000, 5000);
    writeTest<Block::DODF, false>(10U, 300000, 5000);
    writeTest<Block::DO, false>(10U, 300000, 5000);
}

//random write
TEST_F(ObjIntegTest, SEGYRandomWriteSingle1)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<Block::DOMD, false>(200, vec, 117);
    writeRandomTest<Block::DODF, false>(200, vec, 117);
    writeRandomTest<Block::DO, false>(200, vec, 117);
}

TEST_F(ObjIntegTest, SEGYRandomWriteSingle2)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<Block::DOMD, false>(200, vec, 13);
    writeRandomTest<Block::DODF, false>(200, vec, 13);
    writeRandomTest<Block::DO, false>(200, vec, 13);
}

TEST_F(ObjIntegTest, SEGYRandomWriteZeroNt)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(0U, 1337);
    writeRandomTest<Block::DOMD, false>(2000, vec);
    writeRandomTest<Block::DODF, false>(2000, vec);
    writeRandomTest<Block::DO, false>(2000, vec);
}

TEST_F(ObjIntegTest, SEGYRandomWriteZeroNs)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<Block::DOMD, false>(0U, vec);
    writeRandomTest<Block::DODF, false>(0U, vec);
    writeRandomTest<Block::DO, false>(0U, vec);
}

TEST_F(ObjIntegTest, SEGYRandomWrite)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<Block::DOMD, false>(2000, vec);
    writeRandomTest<Block::DODF, false>(2000, vec);
    writeRandomTest<Block::DO, false>(2000, vec);
}

TEST_F(ObjIntegTest, FarmSEGYRandomBigWrite)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(300000U, 1337);
    writeRandomTest<Block::DOMD, false>(5000U, vec);
    writeRandomTest<Block::DODF, false>(5000U, vec);
    writeRandomTest<Block::DO, false>(5000U, vec);
}

