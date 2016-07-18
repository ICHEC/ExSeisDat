#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tglobal.hh"

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

