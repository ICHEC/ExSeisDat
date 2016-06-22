#include <vector>
#include <cassert>
#include "file/iconv.hh"
namespace PIOL {
static bool getConv(iconv_t cd, std::vector<char> & dst, std::vector<char> & src)
{
    size_t oub = dst.size();
    size_t inb = src.size();
    char * pSrc = src.data();
    char * pDst = dst.data();
    ::iconv(cd, &pSrc, &inb, &pDst, &oub);
    return (inb ? false : true);
}

static size_t isGoodChar(char s)
{
    return (s >= 48 && s <= 57)  ||
           (s >= 65 && s <= 90)  ||
           (s >= 97 && s <= 122) ||
           (s == 32) || (s == 9) || (s == 10);
}
static size_t getCount(std::vector<char> & src)
{
    size_t count = 0;
    for (char s : src)
        count += isGoodChar(s);
    return count;
}

IConvert::IConvert()
{
    toAsc = iconv_open("ASCII//", "EBCDICUS//");
    assert(toAsc != (iconv_t)-1);
//        toEbc = iconv_open("ASCII//", "EBCDICUS//",);
//        assert(toEbc != -1);
}
IConvert::~IConvert()
{
    assert(!iconv_close(toAsc));
//        assert(!iconv_close(toEbc));
}

//Do conversion, if more printable letters and spaces etc post-conversion use it.
std::vector<char> IConvert::getAscii(std::vector<char> src)
{
    std::vector<char> dst;
    dst.resize(src.size());
    bool success = getConv(toAsc, dst, src);
    return  ((success) && (getCount(dst) > getCount(src)) ? dst : src);
}
}

