#ifndef TGLOBAL_TEST_INCLUDE_GUARD
#define TGLOBAL_TEST_INCLUDE_GUARD

#include "global.hh"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <cstdlib>
#include <string>

extern const size_t magicNum1;
extern const size_t smallSize;
extern const size_t largeSize;
extern const size_t largeSEGYSize;

extern const std::string notFile;
extern const std::string zeroFile;
extern const std::string smallFile;
extern const std::string largeFile;
// Large file with a pattern
extern const std::string plargeFile;
// File which is deleted after each creation
extern const std::string tempFile;
extern const std::string smallSEGYFile;
extern const std::string largeSEGYFile;
extern const std::string bigTraceSEGYFile;

extern int32_t ilNum(size_t);
extern int32_t xlNum(size_t);
extern PIOL::geom_t xNum(size_t i);
extern PIOL::geom_t yNum(size_t i);
extern PIOL::uchar getPattern(size_t);
extern std::vector<size_t> getRandomVec(size_t nt, int seed);
extern std::vector<size_t> getRandomVec(size_t nt, size_t max, int seed);

constexpr size_t prefix(const size_t pow)
{
    return (pow ? 1024U * prefix(pow - 1U) : 1U);
}

ACTION_P2(check0, buf, sz)
{
    for (size_t i = 0; i < sz; i++)
        ASSERT_EQ(buf[i], arg0[i]) << "Error with byte: " << i << "\n";
}

ACTION_P2(check1, buf, sz)
{
    for (size_t i = 0; i < sz; i++)
        ASSERT_EQ(buf[i], arg1[i]) << "Error with byte: " << i << "\n";
}

ACTION_P2(check2, buf, sz)
{
    for (size_t i = 0; i < sz; i++)
        ASSERT_EQ(buf[i], arg2[i]) << "Error with byte: " << i << "\n";
}

ACTION_P2(check3, buf, sz)
{
    for (size_t i = 0; i < sz; i++)
        ASSERT_EQ(buf[i], arg3[i]) << "Error with byte: " << i << "\n";
}

ACTION_P2(check4, buf, sz)
{
    for (size_t i = 0; i < sz; i++)
        ASSERT_EQ(buf[i], arg4[i]) << "Error with byte: " << i << "\n";
}

#endif
