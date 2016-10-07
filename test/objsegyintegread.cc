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

TEST_F(ObjIntegTest, SEGYDOMDReadSingle)
{
    makeRealSEGY<false>(plargeFile);
    SCOPED_TRACE("Pattern 1");
    readTest<true, false>(10U, 1U, 200, 0, 117);
    SCOPED_TRACE("Pattern 2");
    readTest<true, false>(10U, 1U, 200, 0, 13);
}

TEST_F(ObjIntegTest, SEGYDOMDReadZeroNt)
{
    makeRealSEGY<false>(plargeFile);
    readTest<true, false>(10U, 0U, 2000);
}

TEST_F(ObjIntegTest, SEGYDOMDReadZeroNs)
{
    makeRealSEGY<false>(plargeFile);
    readTest<true, false>(10U, 100U, 0U);
}

TEST_F(ObjIntegTest, SEGYDOMDRead)
{
    makeRealSEGY<false>(plargeFile);
    readTest<true, false>(10U, 100U, 2000);
}

TEST_F(ObjIntegTest, FarmSEGYDOMDBigRead)
{
    makeRealSEGY<false>(plargeFile);
    readTest<true, false>(10U, 300000, 5000);
}

TEST_F(ObjIntegTest, SEGYDODFReadSingle1)
{
    makeRealSEGY<false>(plargeFile);
    readTest<false, false>(10U, 1U, 200, 0, 13);
}

TEST_F(ObjIntegTest, SEGYDODFReadSingle2)
{
    makeRealSEGY<false>(plargeFile);
    readTest<false, false>(10U, 1U, 200, 0, 117);
}

TEST_F(ObjIntegTest, SEGYDODFReadZeroNt)
{
    makeRealSEGY<false>(plargeFile);
    readTest<false, false>(10U, 0U, 2000);
}

TEST_F(ObjIntegTest, SEGYDODFReadZeroNs)
{
    makeRealSEGY<false>(plargeFile);
    readTest<false, false>(10U, 100U, 0U);
}

TEST_F(ObjIntegTest, SEGYDODFRead)
{
    makeRealSEGY<false>(plargeFile);
    readTest<false, false>(10U, 100U, 2000);
}

TEST_F(ObjIntegTest, FarmSEGYDODFBigRead)
{
    makeRealSEGY<false>(plargeFile);
    readTest<false, false>(10U, 300000, 5000);
}

//Random reads
TEST_F(ObjIntegTest, SEGYDOMDRandomReadSingle)
{
    makeRealSEGY<false>(plargeFile);
    SCOPED_TRACE("Pattern 1");
    auto vec = getRandomVec(1U, 1337);
    readRandomTest<true, false>(200U, vec, 117);
    SCOPED_TRACE("Pattern 2");
    readRandomTest<true, false>(200U, vec, 13);
}

TEST_F(ObjIntegTest, SEGYRandomDOMDReadZeroNt)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(0, 1337);
    readRandomTest<true, false>(2000, vec);
}

TEST_F(ObjIntegTest, SEGYRandomDOMDReadZeroNs)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(100U, 1337);
    readRandomTest<true, false>(0U, vec);
}

TEST_F(ObjIntegTest, SEGYRandomDOMDRead)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(100U, 1337);
    readRandomTest<true, false>(2000U, vec);
}

TEST_F(ObjIntegTest, FarmSEGYRandomDOMDBigRead)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(300000, 1337);
    readRandomTest<true, false>(5000U, vec);
}

TEST_F(ObjIntegTest, SEGYRandomDODFReadSingle)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(1U, 1337);
    readRandomTest<false, false>(200, vec, 117);
    readRandomTest<false, false>(200, vec, 13);
}

TEST_F(ObjIntegTest, SEGYRandomDODFReadZeroNt)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(0, 1337);
    readRandomTest<false, false>(2000, vec);
}

TEST_F(ObjIntegTest, SEGYRandomDODFReadZeroNs)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(100U, 1337);
    readRandomTest<false, false>(0U, vec);
}

TEST_F(ObjIntegTest, SEGYRandomDODFRead)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(100U, 1337);
    readRandomTest<false, false>(2000, vec);
}

TEST_F(ObjIntegTest, FarmSEGYRandomDODFBigRead)
{
    makeRealSEGY<false>(plargeFile);
    auto vec = getRandomVec(300000, 1337);
    readRandomTest<false, false>(5000, vec);
}

