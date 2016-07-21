/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "file/iconv.hh"
#include <iconv.h>
#include <vector>
#include <cassert>
#include <cstring>
#include <memory>
namespace PIOL {
/*! \brief Return whether a character is a "good" ASCII character (i.e letters, numbers, spaces etc).
 *  \param[in] s The character
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
 *  \param[sz] sz The size of the string.
 */
static size_t getCount(const char * src, const size_t sz)
{
    size_t count = 0;
    for (size_t i = 0; i < sz; i++)
        count += isGoodChar(src[i]);
    return count;
}

//Do conversion, if more printable letters and spaces etc post-conversion use it.
void getAscii(std::shared_ptr<ExSeisPIOL> piol, const std::string file, uchar * src, const size_t sz)
{
    iconv_t toAsc = iconv_open("ASCII//", "EBCDICUS//");
    piol->log->record(file, Log::Layer::File, Log::Status::Error, "Iconv has failed to open ASCII and EBCDICUS",
                      Log::Verb::None, toAsc == (iconv_t)-1);

    auto dst = std::unique_ptr<char>(new char[sz]);
    size_t isz = sz;
    size_t osz = sz;
    char * in = reinterpret_cast<char *>(src);
    char * out = dst.get();
    ::iconv(toAsc, &in, &isz, &out, &osz);
    piol->log->record(file, Log::Layer::File, Log::Status::Warning, "getConv failed", Log::Verb::None, !osz);

    if (getCount(dst.get(), sz) > getCount(reinterpret_cast<char *>(src), sz))
        std::memcpy(src, dst.get(), sizeof(char)*sz);
    //std::memcpy(src, (getCount(dst.get(), sz) > getCount(pre.get(), sz) ? dst.get() : pre.get()), sizeof(char)*sz);

    int err = iconv_close(toAsc);
    piol->log->record(file, Log::Layer::File, Log::Status::Warning, "Iconv has closed badly", Log::Verb::None, err != 0);
}
}

