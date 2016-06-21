#ifndef PIOLFILEICONV_INCLUDE_GUARD
#define PIOLFILEICONV_INCLUDE_GUARD
#include <vector>
#include <iconv.h>
namespace PIOL {
class IConvert
{
    iconv_t toAsc;
//    iconv_t toEbc;

    public :
    IConvert();
    ~IConvert();
    std::vector<char> getAscii(std::vector<char> src);
};
}
#endif
