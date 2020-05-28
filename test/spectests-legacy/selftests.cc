#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

// Tests to make sure that functions only used in testing are as expected

TEST(Prefix, SizeCheck)
{
    size_t pow = 1U;
    EXPECT_EQ(pow, prefix(0));
    for (size_t i = 1; i <= 6; i++) {
        pow *= 1024U;
        EXPECT_EQ(pow, prefix(i));
    }
}

TEST(Files, SelfTest)
{
    EXPECT_NE(static_cast<size_t>(0), magic_num1);
    EXPECT_EQ(static_cast<size_t>(0), magic_num1 / 0xFF);
    struct stat stats;
    EXPECT_EQ(0, stat(zero_file().c_str(), &stats));
    EXPECT_EQ(0, stat(small_file().c_str(), &stats));
    EXPECT_EQ(0, stat(large_file().c_str(), &stats));
    EXPECT_EQ(0, stat(plarge_file().c_str(), &stats));
}
