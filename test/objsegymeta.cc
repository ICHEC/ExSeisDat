#include "objsegytest.hh"
TEST_F(ObjIntegrationTest, zeroSEGYFileSize)
{
    Obj::SEGY segy(piol, zeroFile, segyOpt, dataOpt);
    piol->isErr();
    EXPECT_NE(nullptr, segy.data);
    EXPECT_EQ(0U, segy.getFileSz());
}

TEST_F(ObjIntegrationTest, SmallSEGYFileSize)
{
    Obj::SEGY segy(piol, smallFile, segyOpt, dataOpt);
    piol->isErr();
    EXPECT_NE(nullptr, segy.data);
    EXPECT_EQ(smallSize, segy.getFileSz());
}

TEST_F(ObjIntegrationTest, BigSEGYFileSize)
{
    Obj::SEGY segy(piol, largeFile, segyOpt, dataOpt);
    piol->isErr();
    EXPECT_NE(nullptr, segy.data);
    EXPECT_EQ(largeSize, segy.getFileSz());
}

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

