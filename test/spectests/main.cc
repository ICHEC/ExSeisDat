#include "tglobal.hh"
#include "cppfileapi.hh"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;
using namespace PIOL;

const size_t magicNum1 = 137; //Number less than 256 that isn't 0.
const size_t smallSize = 4U*prefix(1);
const size_t largeSize = 10U*prefix(3);
const size_t largeSEGYSize = (4U*1000U +240U)*2000000U + 3600U;

const std::string notFile = "!£$%^&*()<>?:@~}{fakefile1234567890";
const std::string zeroFile = "tmp/zeroSizeFile.tmp";
const std::string smallFile = "tmp/smallSizeFile.tmp";
const std::string largeFile = "tmp/largeSizeFile.tmp";
const std::string plargeFile = "tmp/largeFilePattern.tmp";
const std::string tempFile = "tmp/tempFile.tmp";
const std::string smallSEGYFile = "tmp/smallsegy.tmp";
const std::string largeSEGYFile = "tmp/largesegy.tmp";
const std::string bigTraceSEGYFile = "tmp/bigtracesegy.tmp";

int32_t ilNum(size_t i)
{
    return 1600L + (i / 3000L);
}

int32_t xlNum(size_t i)
{
    return 1600L + (i % 3000L);
}

geom_t xNum(size_t i)
{
    return 1000L + (i / 2000L);
}

geom_t yNum(size_t i)
{
    return 1000L + (i % 2000L);
}

void makeFile(std::string name, size_t sz)
{
    static const char zero = '\0';
    FILE * fs = fopen(name.c_str(), "w");

    if (sz != 0)
    {
        //Seek beyond the end of the file and write a single null byte. This ensures the file is all zeroes
        //according to IEEE Std 1003.1-2013
        fseek(fs, sz-1ll, SEEK_SET);
        fwrite(&zero, sizeof(uchar), 1, fs);
    }
    fclose(fs);
}

uchar getPattern(size_t i)
{
    const size_t psz = 0x100;
    i %= psz;
    return i;
}

std::vector<size_t> getRandomVec(size_t nt, size_t max, int seed)
{
    srand(seed);
    if (nt == 0)
        return std::vector<size_t>();

    llint range = (max / nt)-1LL;
    assert(range >= 0);

    std::vector<size_t> v(nt);
    v[0] = (range ? rand() % range : 0);
    for (size_t i = 1; i < nt; i++)
        v[i] = v[i-1] + 1U + (range ? rand() % range : 0);
    return v;
}

std::vector<size_t> getRandomVec(size_t nt, int seed)
{
    return getRandomVec(nt, 12345, seed);
}

int main(int argc, char ** argv)
{
    auto piol = ExSeis::New();
    InitGoogleTest(&argc, argv);
    if (!piol->getRank())
    {
        makeFile(zeroFile, 0U);
        makeFile(smallFile, smallSize);
        makeFile(largeFile, largeSize);
    }
    piol->barrier();

    int code = RUN_ALL_TESTS();

    piol->barrier();
    if (piol->getRank())
    {
        std::remove(zeroFile.c_str());
        std::remove(smallFile.c_str());
        std::remove(largeFile.c_str());
    }

    return code;
}

