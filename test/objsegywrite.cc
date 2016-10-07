#include "objsegytest.hh"
TEST_F(ObjSpecTest, SEGYHOWrite1)
{
    writeHOPattern(0, 107);
}

TEST_F(ObjSpecTest, SEGYHOWrite2)
{
    writeHOPattern(0, 46);
}

TEST_F(ObjSpecTest, SEGYHOWrite3)
{
    writeHOPattern(0, 0);
}

TEST_F(ObjSpecTest, SEGYDOMDWriteSingle1)
{
    writeTest<true>(10U, 1U, 200, 13, 13);
}

TEST_F(ObjSpecTest, SEGYDOMDWriteSingle2)
{
    writeTest<true>(10U, 1U, 200, 13, 117);
}

TEST_F(ObjSpecTest, SEGYDOMDWriteZeroNt)
{
    writeTest<true>(10U, 0U, 2000);
}

TEST_F(ObjSpecTest, SEGYDOMDWriteZeroNs)
{
    writeTest<true>(10U, 100U, 0U);
}

TEST_F(ObjSpecTest, SEGYDOMDWrite)
{
    writeTest<true>(10U, 100U, 2000);
}

TEST_F(ObjSpecTest, FarmSEGYDOMDBigWrite)
{
    writeTest<true>(10U, 300000, 5000);
}

TEST_F(ObjSpecTest, SEGYDODFWriteSingle1)
{
    writeTest<false>(10U, 1U, 200, 13, 13);
}

TEST_F(ObjSpecTest, SEGYDODFWriteSingle2)
{
    writeTest<false>(10U, 1U, 200, 13, 117);
}

TEST_F(ObjSpecTest, SEGYDODFWriteZeroNt)
{
    writeTest<false>(10U, 0U, 2000);
}

TEST_F(ObjSpecTest, SEGYDODFWriteZeroNs)
{
    writeTest<false>(10U, 100U, 0U);
}

TEST_F(ObjSpecTest, SEGYDODFWrite)
{
    writeTest<false>(10U, 100U, 2000);
}

TEST_F(ObjSpecTest, FarmSEGYDODFBigWrite)
{
    writeTest<false>(10U, 300000, 5000);
}

//random read

TEST_F(ObjSpecTest, SEGYRandomDOMDWriteSingle1)
{
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<true>(200, vec, 13);
}

TEST_F(ObjSpecTest, SEGYRandomDOMDWriteSingle2)
{
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<true>(200, vec, 117);
}

TEST_F(ObjSpecTest, SEGYRandomDOMDWriteZeroNt)
{
    auto vec = getRandomVec(0U, 1337);
    writeRandomTest<true>(2000, vec);
}

TEST_F(ObjSpecTest, SEGYRandomDOMDWriteZeroNs)
{
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<true>(0U, vec);
}

TEST_F(ObjSpecTest, SEGYRandomDOMDWrite)
{
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<true>(2000, vec);
}

TEST_F(ObjSpecTest, FarmSEGYRandomDOMDBigWrite)
{
    auto vec = getRandomVec(300000U, 1337);
    writeRandomTest<true>(5000U, vec);
}

TEST_F(ObjSpecTest, SEGYRandomDODFWriteSingle1)
{
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<false>(200U, vec, 13U);
}

TEST_F(ObjSpecTest, SEGYRandomDODFWriteSingle2)
{
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<false>(200, vec, 117);
}

TEST_F(ObjSpecTest, SEGYRandomDODFWriteZeroNt)
{
    auto vec = getRandomVec(0U, 1337);
    writeRandomTest<false>(2000, vec);
}

TEST_F(ObjSpecTest, SEGYRandomDODFWriteZeroNs)
{
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<false>(0U, vec);
}

TEST_F(ObjSpecTest, SEGYRandomDODFWrite)
{
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<false>(2000, vec);
}

TEST_F(ObjSpecTest, FarmSEGYRandomDODFBigWrite)
{
    auto vec = getRandomVec(300000U, 1337);
    writeRandomTest<false>(5000U, vec);
}
