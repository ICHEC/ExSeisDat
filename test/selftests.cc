#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"
#include <iostream>
//Tests to make sure that functions only used in testing are as expected

TEST(Prefix, SizeCheck)
{
    size_t pow = 1U;
    EXPECT_EQ(pow, prefix(0));
    for (size_t i = 1; i <= 6; i++)
    {
        pow *= 1024U;
        EXPECT_EQ(pow, prefix(i));
    }
}

TEST(Files, SelfTest)
{
    EXPECT_NE(0, magicNum1);
    EXPECT_EQ(0, magicNum1 / 0xFF);
    struct stat stats;
    EXPECT_EQ(0, stat(zeroFile.c_str(), &stats));
    EXPECT_EQ(0, stat(smallFile.c_str(), &stats));
    EXPECT_EQ(0, stat(largeFile.c_str(), &stats));
    EXPECT_EQ(0, stat(plargeFile.c_str(), &stats));
}

