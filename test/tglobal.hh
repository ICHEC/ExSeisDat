#ifndef TGLOBAL_TEST_INCLUDE_GUARD
#define TGLOBAL_TEST_INCLUDE_GUARD

#include <string>
#include <cstdlib>
#include "global.hh"
extern const size_t magicNum1;
extern const size_t smallSize;
extern const size_t largeSize;

extern const std::string notFile;
extern const std::string zeroFile;
extern const std::string smallFile;
extern const std::string largeFile;
extern const std::string plargeFile;

extern uchar getPattern(size_t);
constexpr size_t prefix(const size_t pow)
{
    return (pow ? 1024U*prefix(pow-1U) : 1U);
}
#endif
