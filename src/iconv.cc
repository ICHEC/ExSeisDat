/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include <iconv.h>
#include <cstring>
#include "file/iconv.hh"
namespace PIOL {
/*! \brief Return whether a character is a "good" ASCII character (i.e letters, numbers, spaces etc).
 *  \param[in] s The character
 *  \return True if the character is "good" (i.e typically used by a person leaving messages).
 */
static bool isGoodChar(char s)
{
    return (s >= 48 && s <= 57)  ||
           (s >= 65 && s <= 90)  ||
           (s >= 97 && s <= 122) ||
           (s == 32) || (s == 9) || (s == 10);
}

/*! \brief The number of standard characters in the ASCII string (i.e letters, numbers, spaces etc).
 *  \param[in] src The string.
 *  \param[in] sz The size of the string.
 *  \return The number of standard characters in the string.
 */
static size_t getCount(const char * src, csize_t sz)
{
    size_t count = 0;
    for (size_t i = 0; i < sz; i++)
        count += isGoodChar(src[i]);
    return count;
}

//Do conversion, if more printable letters and spaces etc post-conversion use it.
void getAscii(ExSeisPIOL * piol, const std::string file, csize_t sz, uchar * src)
{
    iconv_t toAsc = iconv_open("ASCII//", "EBCDICUS//");
    piol->log->record(file, Log::Layer::File, Log::Status::Error, "Iconv has failed to open ASCII and EBCDICUS",
                      Log::Verb::None, toAsc == (iconv_t)-1);

    std::vector<char> dst(sz);
    size_t isz = sz;
    size_t osz = sz;
    char * in = reinterpret_cast<char *>(src);
    char * out = dst.data();

    size_t serr = ::iconv(toAsc, &in, &isz, &out, &osz);
    if (serr == size_t(-1) || osz)
    {
        piol->log->record(file, Log::Layer::File, Log::Status::Warning, "getConv osz wasn't zero or iconv failed", Log::Verb::Max, !osz);
    }
    else if (getCount(dst.data(), sz) > getCount(reinterpret_cast<char *>(src), sz))
        std::memcpy(src, dst.data(), sizeof(char)*sz);
    int err = iconv_close(toAsc);
    piol->log->record(file, Log::Layer::File, Log::Status::Warning, "Iconv has closed badly", Log::Verb::None, err != 0);
}
}

