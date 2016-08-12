#include "objsegytest.hh"
TEST_F(ObjSpecTest, SEGYDOMDWriteSingle)
{
    makeSEGY();
    writeTest<true>(10U, 1U, 200, 13, 117);
    writeTest<true>(10U, 1U, 200, 13, 13);
}

TEST_F(ObjSpecTest, SEGYDOMDWriteZeroNt)
{
    makeSEGY();
    writeTest<true>(10U, 0U, 2000);
}

TEST_F(ObjSpecTest, SEGYDOMDWriteZeroNs)
{
    makeSEGY();
    writeTest<true>(10U, 100U, 0U);
}

TEST_F(ObjSpecTest, SEGYDOMDWrite)
{
    makeSEGY();
    writeTest<true>(10U, 100U, 2000);
}

TEST_F(ObjSpecTest, FarmSEGYDOMDBigWrite)
{
    makeSEGY();
    writeTest<true>(10U, 300000, 5000);
}

TEST_F(ObjSpecTest, SEGYDODFWriteSingle)
{
    makeSEGY();
    writeTest<false>(10U, 1U, 200, 13, 117);
    writeTest<false>(10U, 1U, 200, 13, 13);
}

TEST_F(ObjSpecTest, SEGYDODFWriteZeroNt)
{
    makeSEGY();
    writeTest<false>(10U, 0U, 2000);
}

TEST_F(ObjSpecTest, SEGYDODFWriteZeroNs)
{
    makeSEGY();
    writeTest<false>(10U, 100U, 0U);
}

TEST_F(ObjSpecTest, SEGYDODFWrite)
{
    makeSEGY();
    writeTest<false>(10U, 100U, 2000);
}

TEST_F(ObjSpecTest, FarmSEGYDODFBigWrite)
{
    makeSEGY();
    writeTest<false>(10U, 300000, 5000);
}
