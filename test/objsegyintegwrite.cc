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

TEST_F(ObjIntegTest, SEGYDOMDWriteSingle1)
{
    makeRealSEGY<true>(tempFile);
    writeTest<true, false>(10U, 1U, 200, 0, 117);
}

TEST_F(ObjIntegTest, SEGYDOMDWriteSingle2)
{
    makeRealSEGY<true>(tempFile);
    writeTest<true, false>(10U, 1U, 200, 0, 13);
}

TEST_F(ObjIntegTest, SEGYDOMDWriteZeroNt)
{
    makeRealSEGY<true>(tempFile);
    writeTest<true, false>(10U, 0U, 2000);
}

TEST_F(ObjIntegTest, SEGYDOMDWriteZeroNs)
{
    makeRealSEGY<true>(tempFile);
    writeTest<true, false>(10U, 100U, 0U);
}

TEST_F(ObjIntegTest, SEGYDOMDWrite)
{
    makeRealSEGY<true>(tempFile);
    writeTest<true, false>(10U, 100U, 2000);
}

TEST_F(ObjIntegTest, FarmSEGYDOMDBigWrite)
{
    makeRealSEGY<true>(tempFile);
    writeTest<true, false>(10U, 300000, 5000);
}

TEST_F(ObjIntegTest, SEGYDODFWriteSingle)
{
    makeRealSEGY<true>(tempFile);
    writeTest<false, false>(10U, 1U, 200, 0, 117);
    writeTest<false, false>(10U, 1U, 200, 0, 13);
}

TEST_F(ObjIntegTest, SEGYDODFWriteZeroNt)
{
    makeRealSEGY<true>(tempFile);
    writeTest<false, false>(10U, 0U, 2000);
}

TEST_F(ObjIntegTest, SEGYDODFWriteZeroNs)
{
    makeRealSEGY<true>(tempFile);
    writeTest<false, false>(10U, 100U, 0U);
}

TEST_F(ObjIntegTest, SEGYDODFWrite)
{
    makeRealSEGY<true>(tempFile);
    writeTest<false, false>(10U, 100U, 2000);
}

TEST_F(ObjIntegTest, FarmSEGYDODFBigWrite)
{
    makeRealSEGY<true>(tempFile);
    writeTest<false, false>(10U, 300000, 5000);
}

//random read
TEST_F(ObjIntegTest, SEGYRandomDOMDWriteSingle1)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<true, false>(200, vec, 117);
}

TEST_F(ObjIntegTest, SEGYRandomDOMDWriteSingle2)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<true, false>(200, vec, 13);
}

TEST_F(ObjIntegTest, SEGYRandomDOMDWriteZeroNt)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(0U, 1337);
    writeRandomTest<true, false>(2000, vec);
}

TEST_F(ObjIntegTest, SEGYRandomDOMDWriteZeroNs)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<true, false>(0U, vec);
}

TEST_F(ObjIntegTest, SEGYRandomDOMDWrite)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<true, false>(2000, vec);
}

TEST_F(ObjIntegTest, FarmSEGYRandomDOMDBigWrite)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(300000U, 1337);
    writeRandomTest<true, false>(5000U, vec);
}

TEST_F(ObjIntegTest, SEGYRandomDODFWriteSingle)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(1U, 1337);
    writeRandomTest<false, false>(200, vec, 117);
    writeRandomTest<false, false>(200, vec, 13);
}

TEST_F(ObjIntegTest, SEGYRandomDODFWriteZeroNt)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(0U, 1337);
    writeRandomTest<false, false>(2000, vec);
}

TEST_F(ObjIntegTest, SEGYRandomDODFWriteZeroNs)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<false, false>(0U, vec);
}

TEST_F(ObjIntegTest, SEGYRandomDODFWrite)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(100U, 1337);
    writeRandomTest<false, false>(2000, vec);
}

TEST_F(ObjIntegTest, FarmSEGYRandomDODFBigWrite)
{
    makeRealSEGY<true>(tempFile);
    auto vec = getRandomVec(300000U, 1337);
    writeRandomTest<false, false>(5000U, vec);
}

