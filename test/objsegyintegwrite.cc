#include "objsegytest.hh"
TEST_F(ObjIntegTest, SEGYHOWrite)
{
    makeRealSEGY<true>(tempFile);
    SCOPED_TRACE("Pattern 1");
    writeHOPattern<true>(0, 107);

    SCOPED_TRACE("Pattern 2");
    writeHOPattern<true>(0, 46);

    SCOPED_TRACE("Pattern 3");
    writeHOPattern<true>(0, 0);
}

TEST_F(ObjIntegTest, SEGYDOMDWriteSingle)
{
    makeRealSEGY<true>(tempFile);
    SCOPED_TRACE("Pattern 1");
    writeTest<true, false>(10U, 1U, 200, 0, 117);
    SCOPED_TRACE("Pattern 2");
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
