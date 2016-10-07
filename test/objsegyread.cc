#include "objsegytest.hh"
TEST_F(ObjSpecTest, SEGYHORead1)
{
    readHOPatternTest(7, 107);
}

TEST_F(ObjSpecTest, SEGYHORead2)
{
    readHOPatternTest(10, 46);
}

TEST_F(ObjSpecTest, SEGYHORead3)
{
    readHOPatternTest(0, 0);
}

TEST_F(ObjSpecTest, SEGYReadSingle1)
{
    readTest<Block::DOMD>(10U, 1U, 200, 13, 117);
    readTest<Block::DODF>(10U, 1U, 200, 13, 117);
    readTest<Block::DO>(10U, 1U, 200, 13, 117);
}

TEST_F(ObjSpecTest, SEGYReadSingle2)
{
    readTest<Block::DOMD>(10U, 1U, 200, 13, 13);
    readTest<Block::DODF>(10U, 1U, 200, 13, 13);
    readTest<Block::DO>(10U, 1U, 200, 13, 13);
}

TEST_F(ObjSpecTest, SEGYReadZeroNt)
{
    readTest<Block::DOMD>(10U, 0U, 2000);
    readTest<Block::DODF>(10U, 0U, 2000);
    readTest<Block::DO>(10U, 0U, 2000);
}

TEST_F(ObjSpecTest, SEGYReadZeroNs)
{
    readTest<Block::DOMD>(10U, 100U, 0U);
    readTest<Block::DODF>(10U, 100U, 0U);
    readTest<Block::DO>(10U, 100U, 0U);
}

TEST_F(ObjSpecTest, SEGYRead)
{
    readTest<Block::DOMD>(10U, 100U, 2000);
    readTest<Block::DODF>(10U, 100U, 2000);
    readTest<Block::DO>(10U, 100U, 2000);
}

TEST_F(ObjSpecTest, FarmSEGYBigRead)
{
    readTest<Block::DOMD>(10U, 300000, 5000);
    readTest<Block::DODF>(10U, 300000, 5000);
    readTest<Block::DO>(10U, 300000, 5000);
}

//random read

TEST_F(ObjSpecTest, SEGYRandomReadSingle1)
{
    auto vec = getRandomVec(1U, 1337);
    readRandomTest<Block::DOMD>(200, vec, 117);
    readRandomTest<Block::DODF>(200, vec, 117);
    readRandomTest<Block::DO>(200, vec, 117);
}

TEST_F(ObjSpecTest, SEGYRandomReadSingle2)
{
    auto vec = getRandomVec(1U, 1337);
    readRandomTest<Block::DOMD>(200, vec, 13);
    readRandomTest<Block::DODF>(200, vec, 13);
    readRandomTest<Block::DO>(200, vec, 13);
}

TEST_F(ObjSpecTest, SEGYRandomReadZeroNt)
{
    auto vec = getRandomVec(0U, 1337);
    readRandomTest<Block::DOMD>(2000, vec);
    readRandomTest<Block::DODF>(2000, vec);
    readRandomTest<Block::DO>(2000, vec);
}

TEST_F(ObjSpecTest, SEGYRandomReadZeroNs)
{
    auto vec = getRandomVec(100U, 1337);
    readRandomTest<Block::DOMD>(0U, vec);
    readRandomTest<Block::DODF>(0U, vec);
    readRandomTest<Block::DO>(0U, vec);
}

TEST_F(ObjSpecTest, SEGYRandomRead)
{
    auto vec = getRandomVec(100U, 1337);
    readRandomTest<Block::DOMD>(2000, vec);
    readRandomTest<Block::DODF>(2000, vec);
    readRandomTest<Block::DO>(2000, vec);
}

TEST_F(ObjSpecTest, FarmSEGYRandomBigRead)
{
    auto vec = getRandomVec(300000U, 1337);
    readRandomTest<Block::DOMD>(5000, vec);
    readRandomTest<Block::DODF>(5000, vec);
    readRandomTest<Block::DO>(5000, vec);
}

