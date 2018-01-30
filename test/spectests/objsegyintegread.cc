#include "objsegytest.hh"
TEST_F(ObjIntegTest, SEGYHORead1)
{
    makeRealSEGY<false>(plargeFile);
    readHOPatternTest<false>(0, 107);
}

TEST_F(ObjIntegTest, SEGYHORead2)
{
    makeRealSEGY<false>(plargeFile);
    readHOPatternTest<false>(0, 46);
}

TEST_F(ObjIntegTest, SEGYHORead3)
{
    makeRealSEGY<false>(plargeFile);
    readHOPatternTest<false>(0, 0);
}

TEST_F(ObjIntegTest, SEGYReadSingle1)
{
    makeRealSEGY<false>(plargeFile);
    readTest<Block::DOMD, false>(10U, 1U, 200, 0, 13);
    readTest<Block::DODF, false>(10U, 1U, 200, 0, 13);
    readTest<Block::DO, false>(10U, 1U, 200, 0, 13);
}

TEST_F(ObjIntegTest, SEGYReadSingle2)
{
    makeRealSEGY<false>(plargeFile);
    readTest<Block::DOMD, false>(10U, 1U, 200, 0, 117);
    readTest<Block::DODF, false>(10U, 1U, 200, 0, 117);
    readTest<Block::DO, false>(10U, 1U, 200, 0, 117);
}

TEST_F(ObjIntegTest, SEGYReadZeroNt)
{
    makeRealSEGY<false>(plargeFile);
    readTest<Block::DOMD, false>(10U, 0U, 2000);
    readTest<Block::DODF, false>(10U, 0U, 2000);
    readTest<Block::DO, false>(10U, 0U, 2000);
}

TEST_F(ObjIntegTest, SEGYReadZeroNs)
{
    makeRealSEGY<false>(plargeFile);
    readTest<Block::DOMD, false>(10U, 100U, 0U);
    readTest<Block::DODF, false>(10U, 100U, 0U);
    readTest<Block::DO, false>(10U, 100U, 0U);
}

TEST_F(ObjIntegTest, SEGYRead)
{
    makeRealSEGY<false>(plargeFile);
    readTest<Block::DOMD, false>(10U, 100U, 2000);
    readTest<Block::DODF, false>(10U, 100U, 2000);
    readTest<Block::DO, false>(10U, 100U, 2000);
}

TEST_F(ObjIntegTest, FarmSEGYBigRead)
{
    makeRealSEGY<false>(plargeFile);
    readTest<Block::DOMD, false>(10U, 300000, 5000);
    readTest<Block::DODF, false>(10U, 300000, 5000);
    readTest<Block::DO, false>(10U, 300000, 5000);
}

//Random reads
TEST_F(ObjIntegTest, SEGYRandomReadSingle1)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(1U, 1337);
    readRandomTest<Block::DOMD, false>(200U, vec, 117);
    readRandomTest<Block::DODF, false>(200U, vec, 117);
    readRandomTest<Block::DO, false>(200U, vec, 117);
}

TEST_F(ObjIntegTest, SEGYRandomReadSingle2)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(1U, 1337);
    readRandomTest<Block::DOMD, false>(200U, vec, 13);
    readRandomTest<Block::DODF, false>(200U, vec, 13);
    readRandomTest<Block::DO, false>(200U, vec, 13);
}

TEST_F(ObjIntegTest, SEGYRandomReadZeroNt)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(0, 1337);
    readRandomTest<Block::DOMD, false>(2000, vec);
    readRandomTest<Block::DODF, false>(2000, vec);
    readRandomTest<Block::DO, false>(2000, vec);
}

TEST_F(ObjIntegTest, SEGYRandomReadZeroNs)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(100U, 1337);
    readRandomTest<Block::DOMD, false>(0U, vec);
    readRandomTest<Block::DODF, false>(0U, vec);
    readRandomTest<Block::DO, false>(0U, vec);
}

TEST_F(ObjIntegTest, SEGYRandomRead)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(100U, 1337);
    readRandomTest<Block::DOMD, false>(2000U, vec);
    readRandomTest<Block::DODF, false>(2000U, vec);
    readRandomTest<Block::DO, false>(2000U, vec);
}

TEST_F(ObjIntegTest, FarmSEGYRandomBigRead)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(300000, 1337);
    readRandomTest<Block::DOMD, false>(5000U, vec);
    readRandomTest<Block::DODF, false>(5000U, vec);
    readRandomTest<Block::DO, false>(5000U, vec);
}

