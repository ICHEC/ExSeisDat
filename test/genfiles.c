#include <stdio.h>
typedef unsigned char uchar;

void writePattern(FILE * fs, const size_t sz, const uchar const * pattern, const size_t psz)
{
    size_t q = sz / psz;
    size_t r = sz % psz;
    for (size_t i = 0; i < q; i++)
        fwrite(pattern, sizeof(uchar), psz, fs);
    if (r)
        fwrite(pattern, sizeof(uchar), r, fs);
}

void makeFile(const char const * name, const size_t sz, const uchar const * pattern, const size_t psz)
{
    FILE * fs = fopen(name, "w");
    if (sz == 0)
    {
        fclose(fs);
        return;
    }

    fseek(fs, 0U, SEEK_SET);
    writePattern(fs, sz, pattern, psz);
    fclose(fs);
}

void makeSEGY(const char const * out, const size_t ns, const size_t nt, const uchar const * pattern, const size_t psz)
{
    const size_t hsz  = 3600;
    const size_t thsz = 240U;
    size_t sz = hsz + (thsz + ns*sizeof(float)) * nt;
    FILE * fs = fopen(out, "w");

    if (sz >= hsz)
    {
        uchar cns[2U] = {(ns & 0xFF00) >> 8U, ns & 0xFF};

        fseek(fs, 0U, SEEK_SET);
        writePattern(fs, 3200U, pattern, psz);

        fseek(fs, 3220U, SEEK_SET);
        fwrite(cns, sizeof(uchar), 2U, fs);

        fseek(fs, 3600U, SEEK_SET);
        sz -= hsz;

        writePattern(fs, sz, pattern, psz);
        fclose(fs);
    }
}

int main(void)
{
    const size_t psz = 0x100;
    uchar pattern[psz];
    for (size_t i = 0; i < psz; i++)
        pattern[i] = i % psz;
        //pattern[i] = i + i % 3 + i % 9 + i % (psz - 7);

    makeFile("tmp/smallFilePattern.tmp", 4096ll, pattern, psz);
    makeFile("tmp/largeFilePattern.tmp", 10ll*1024ll*1024ll*1024ll, pattern, psz);

    makeSEGY("tmp/smallsegy.tmp", 261U, 400U, pattern, psz);
    return 0;
}

