#include <string>
#include <cstdlib>

extern const size_t magicNum1;
extern const size_t smallSize;
extern const size_t largeSize;

extern const std::string zeroFile;
extern const std::string smallFile;
extern const std::string largeFile;
extern const std::string plargeFile;

constexpr size_t prefix(const size_t pow)
{
    return (pow ? 1024U*prefix(pow-1U) : 1U);
}

