#include "objsegytest.hh"

TEST_F(ObjSpecTest, TestBypassConstructor)
{
    makeSEGY();
    EXPECT_EQ(piol, obj->piol());
    EXPECT_EQ(notFile, obj->name());
    EXPECT_EQ(mock, obj->data());
}

TEST_F(ObjIntegTest, zeroSEGYFileSize)
{
    makeRealSEGY<false>(zeroFile);
    piol->isErr();
    EXPECT_NE(nullptr, obj->data());
    EXPECT_EQ(0U, obj->getFileSz());
    piol->isErr();
}

TEST_F(ObjIntegTest, SmallSEGYFileSize)
{
    makeRealSEGY<false>(smallFile);
    piol->isErr();
    EXPECT_NE(nullptr, obj->data());
    EXPECT_EQ(smallSize, obj->getFileSz());
    piol->isErr();
}

TEST_F(ObjIntegTest, BigSEGYFileSize)
{
    makeRealSEGY<false>(largeFile);
    piol->isErr();
    EXPECT_NE(nullptr, obj->data());
    EXPECT_EQ(largeSize, obj->getFileSz());
    piol->isErr();
}

TEST_F(ObjSpecTest, ZeroSEGYFileSize)
{
    makeSEGY();
    SEGYFileSizeTest(0U);
}

TEST_F(ObjSpecTest, SmallSEGYFileSize)
{
    makeSEGY();
    SEGYFileSizeTest(40U * prefix(2U));
}

TEST_F(ObjSpecTest, BigSEGYFileSize)
{
    makeSEGY();
    SEGYFileSizeTest(8U * prefix(4U));
}
