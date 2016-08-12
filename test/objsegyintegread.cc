#include "objsegytest.hh"
TEST_F(ObjIntegTest, SEGYHORead)
{
    makeRealSEGY<false>(plargeFile);
    SCOPED_TRACE("Pattern 1");
    readHOPatternTest<false>(0, 107);
    SCOPED_TRACE("Pattern 2");
    readHOPatternTest<false>(0, 46);
    SCOPED_TRACE("Pattern 3");
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

TEST_F(ObjIntegTest, SEGYDODFReadSingle)
{
    makeRealSEGY<false>(plargeFile);
    readTest<false, false>(10U, 1U, 200, 0, 117);
    readTest<false, false>(10U, 1U, 200, 0, 13);
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
