#ifndef PIOLFILESEGYICONV_INCLUDE_GUARD
#define PIOLFILESEGYICONV_INCLUDE_GUARD
#include <vector>
namespace PIOL { namespace File { namespace SEGY {
class iconvert
{
    iconv_t toAsc;
//    iconv_t toEbc;

    void getConv(iconv_t cd, std::vector<char> & dst, std::vector<char> & src)
    {
        size_t oub = dst.size();
        size_t inb = src.size();
        char * pSrc = src.data();
        char * pDst = dst.data();
        ::iconv(cd, &pSrc, &inb, &pDst, &oub);
        assert(!oub);
        assert(!inb);
    }

    public :
    iconvert()
    {
        toAsc = iconv_open("ASCII//", "EBCDICUS//");
        assert(toAsc != (iconv_t)-1);
//        toEbc = iconv_open("ASCII//", "EBCDICUS//",);
//        assert(toEbc != -1);
    }
    ~iconvert()
    {
        assert(!iconv_close(toAsc));
//        assert(!iconv_close(toEbc));
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
//Do conversion, if more printable letters and spaces etc post-conversion use it.
    std::vector<char> getAscii(std::vector<char> src)
    {
        std::vector<char> dst;
        dst.resize(src.size());
        getConv(toAsc, dst, src);
        if (getCount(src) > getCount(dst))
            return src;
        return dst;
    }
};
}}}
#endif
