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

TEST_F(ObjSpecTest, SEGYDOMDReadSingle1)
{
    readTest<true>(10U, 1U, 200, 13, 117);
}

TEST_F(ObjSpecTest, SEGYDOMDReadSingle2)
{
    readTest<true>(10U, 1U, 200, 13, 13);
}

TEST_F(ObjSpecTest, SEGYDOMDReadZeroNt)
{
    readTest<true>(10U, 0U, 2000);
}

TEST_F(ObjSpecTest, SEGYDOMDReadZeroNs)
{
    readTest<true>(10U, 100U, 0U);
}

TEST_F(ObjSpecTest, SEGYDOMDRead)
{
    readTest<true>(10U, 100U, 2000);
}

TEST_F(ObjSpecTest, FarmSEGYDOMDBigRead)
{
    readTest<true>(10U, 300000, 5000);
}

TEST_F(ObjSpecTest, SEGYDODFReadSingle1)
{
    readTest<false>(10U, 1U, 200, 13, 117);
}

TEST_F(ObjSpecTest, SEGYDODFReadSingle2)
{
    readTest<false>(10U, 1U, 200, 13, 13);
}

TEST_F(ObjSpecTest, SEGYDODFReadZeroNt)
{
    readTest<false>(10U, 0U, 2000);
}

TEST_F(ObjSpecTest, SEGYDODFReadZeroNs)
{
    readTest<false>(10U, 100U, 0U);
}

TEST_F(ObjSpecTest, SEGYDODFRead)
{
    readTest<false>(10U, 100U, 2000);
}

TEST_F(ObjSpecTest, FarmSEGYDODFBigRead)
{
    readTest<false>(10U, 300000, 5000);
}

//random read

TEST_F(ObjSpecTest, SEGYRandomDOMDReadSingle1)
{
    auto vec = getRandomVec(1U, 1337);
    readTest<true>(10U, 1U, 200, 13, 117);
}

TEST_F(ObjSpecTest, SEGYRandomDOMDReadSingle2)
{
    auto vec = getRandomVec(1U, 1337);
    readTest<true>(10U, 1U, 200, 13, 13);
}

TEST_F(ObjSpecTest, SEGYRandomDOMDReadZeroNt)
{
    auto vec = getRandomVec(0U, 1337);
    readTest<true>(10U, 0U, 2000);
}

TEST_F(ObjSpecTest, SEGYRandomDOMDReadZeroNs)
{
    auto vec = getRandomVec(100U, 1337);
    readTest<true>(10U, 100U, 0U);
}

TEST_F(ObjSpecTest, SEGYRandomDOMDRead)
{
    auto vec = getRandomVec(100U, 1337);
    readTest<true>(10U, 100U, 2000);
}

TEST_F(ObjSpecTest, FarmSEGYRandomDOMDBigRead)
{
    auto vec = getRandomVec(300000U, 1337);
    readTest<true>(10U, 300000U, 5000);
}

TEST_F(ObjSpecTest, SEGYRandomDODFReadSingle1)
{
    auto vec = getRandomVec(1U, 1337);
    readTest<false>(10U, 1U, 200, 13, 117);
}

TEST_F(ObjSpecTest, SEGYRandomDODFReadSingle2)
{
    auto vec = getRandomVec(1U, 1337);
    readTest<false>(10U, 1U, 200U, 13, 13);
}

TEST_F(ObjSpecTest, SEGYRandomDODFReadZeroNt)
{
    auto vec = getRandomVec(0U, 1337);
    readTest<false>(10U, 0U, 2000U);
}

TEST_F(ObjSpecTest, SEGYRandomDODFReadZeroNs)
{
    auto vec = getRandomVec(100U, 1337);
    readTest<false>(10U, 100U, 0U);
}

TEST_F(ObjSpecTest, SEGYRandomDODFRead)
{
    auto vec = getRandomVec(100U, 1337);
    readTest<false>(10U, 100U, 2000);
}

TEST_F(ObjSpecTest, FarmSEGYRandomDODFBigRead)
{
    auto vec = getRandomVec(300000U, 1337);
    readTest<false>(10U, 300000U, 5000);
}
