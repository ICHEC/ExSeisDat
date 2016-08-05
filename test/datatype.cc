#include "share/datatype.hh"
#include <stdint.h>
#include <vector>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
/*
template <typename T, typename std::enable_if<sizeof(T) == 2U, T>::type = 0>
T getHost(const uchar * src)

template <typename T, typename std::enable_if<sizeof(T) == 4U, T>::type = 0>
void getBigEndian(const T src, uchar * dst)

template <typename T, typename std::enable_if<sizeof(T) == 2U, T>::type = 0>
void getBigEndian(const T src, uchar * dst)
*/
using namespace PIOL;
TEST(Datatype, getHost32Bit1)
{
    std::vector<uchar> src = {1, 1, 1, 1};
    auto h = getHost<int32_t>(src.data());
    EXPECT_EQ(16843009, h);
}

TEST(Datatype, getHost32Bit2)
{
    std::vector<uchar> src = {0x78, 0x9A, 0xBC, 0xDE};
    auto h = getHost<int32_t>(src.data());
    EXPECT_EQ(2023406814, h);
}

TEST(Datatype, getHost32Bit3)
{
    std::vector<uchar> src = {0x88, 0x9A, 0xBC, 0xDE};
    auto h = getHost<int32_t>(src.data());

    int32_t ans = 0x889ABCDE;
    EXPECT_EQ(ans, h);
}

