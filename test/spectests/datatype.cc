#include "share/datatype.hh"
#include <stdint.h>
#include <vector>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

//include two very large vectors
#include "ibm.hh"
#include "ibm-compare.hh"

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

std::string printBinary(uint32_t val)
{
    std::stringstream s;
    s << "Number = ";
    for (int i = 31; i >= 0; i--)
    {
        if (!((i + 1) % 4))
            s << " ";
        s <<((val >> i) & 0x1);
    }
    s << "\n";
    return s.str();
}

// Small function for getting the size of a static array
template<typename T, size_t N>
static size_t array_size(T (&)[N]) { return N; }

TEST(Datatype, IBMToIEEE)
{
    ASSERT_EQ(array_size(rawTraces), array_size(tktraces));
    for (size_t i = 0; i < array_size(rawTraces); i++)
    {
        float val = convertIBMtoIEEE(tofloat(rawTraces[i]), true);
        EXPECT_EQ(val, tktraces[i]) << "float number " << i
            << "\n raw " << printBinary(rawTraces[i])
            << "me   " << printBinary(toint(convertIBMtoIEEE(tofloat(rawTraces[i]), true)))
            << "them " << printBinary(*reinterpret_cast<const uint32_t *>(&tktraces[i]));
    }
}
