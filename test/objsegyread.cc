#include "objsegytest.hh"
TEST_F(ObjSpecTest, TestBypassConstructor)
{
    makeSEGY();
    EXPECT_EQ(piol, obj->piol);
    EXPECT_EQ(notFile, obj->name);
    EXPECT_EQ(mock, obj->data);
}

TEST_F(ObjSpecTest, ZeroSEGYFileSize)
{
    makeSEGY();
    SEGYFileSizeTest(0U);
}

TEST_F(ObjSpecTest, SmallSEGYFileSize)
{
    makeSEGY();
    SEGYFileSizeTest(40U*prefix(2U));
}

TEST_F(ObjSpecTest, BigSEGYFileSize)
{
    makeSEGY();
    SEGYFileSizeTest(8U*prefix(4U));
}

TEST_F(ObjSpecTest, SEGYHORead)
{
    makeSEGY();
    HOPatternTest(7, 107);
    HOPatternTest(10, 46);
    HOPatternTest(0, 0);
}

TEST_F(ObjSpecTest, SEGYDOMDReadSingle)
{
    makeSEGY();
    readTest<true>(10U, 1U, 200, 13, 117);
    readTest<true>(10U, 1U, 200, 13, 13);
}

TEST_F(ObjSpecTest, SEGYDOMDReadZeroNt)
{
    makeSEGY();
    readTest<true>(10U, 0U, 2000);
}

TEST_F(ObjSpecTest, SEGYDOMDReadZeroNs)
{
    makeSEGY();
    readTest<true>(10U, 100U, 0U);
}

TEST_F(ObjSpecTest, SEGYDOMDRead)
{
    makeSEGY();
    readTest<true>(10U, 100U, 2000);
}

TEST_F(ObjSpecTest, SEGYDOMDBigRead)
{
    makeSEGY();
    readTest<true>(10U, 300000, 5000);
}

TEST_F(ObjSpecTest, SEGYDODFReadSingle)
{
    makeSEGY();
    readTest<false>(10U, 1U, 200, 13, 117);
    readTest<false>(10U, 1U, 200, 13, 13);
}

TEST_F(ObjSpecTest, SEGYDODFReadZeroNt)
{
    makeSEGY();
    readTest<false>(10U, 0U, 2000);
}

TEST_F(ObjSpecTest, SEGYDODFReadZeroNs)
{
    makeSEGY();
    readTest<false>(10U, 100U, 0U);
}

TEST_F(ObjSpecTest, SEGYDODFRead)
{
    makeSEGY();
    readTest<false>(10U, 100U, 2000);
}

TEST_F(ObjSpecTest, FarmSEGYDODFBigRead)
{
    makeSEGY();
    readTest<false>(10U, 300000, 5000);
}
