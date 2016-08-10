#ifndef TGLOBAL_TEST_INCLUDE_GUARD
#define TGLOBAL_TEST_INCLUDE_GUARD

#include <string>
#include <cstdlib>
#include "global.hh"
extern const size_t magicNum1;
extern const size_t smallSize;
extern const size_t largeSize;
extern const size_t largeSEGYSize;

extern const std::string notFile;
extern const std::string zeroFile;
extern const std::string smallFile;
extern const std::string largeFile;
extern const std::string plargeFile;    //Large file with a pattern
extern const std::string tempFile;      //File which is deleted after each creation
extern const std::string smallSEGYFile;
extern const std::string largeSEGYFile;
extern const std::string bigTraceSEGYFile;

extern int32_t ilNum(size_t);
extern int32_t xlNum(size_t);
extern geom_t xNum(size_t i);
extern geom_t yNum(size_t i);
extern uchar getPattern(size_t);

constexpr size_t prefix(const size_t pow)
{
    return (pow ? 1024U*prefix(pow-1U) : 1U);
}
#endif
