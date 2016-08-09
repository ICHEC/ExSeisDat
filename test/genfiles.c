#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
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

int32_t ilNum(size_t i)
{
    return 1600L + (i / 3000L);
}

int32_t xlNum(size_t i)
{
    return 1600L + (i % 3000L);
}

void makeSEGY(const char const * out, const size_t ns, const size_t nt, size_t maxBlock)
{
    const size_t hsz  = 3600;
    const size_t thsz = 240U;
    size_t dosz = (thsz + ns*sizeof(float));
    size_t sz = hsz + (thsz + ns*sizeof(float)) * nt;
    FILE * fs = fopen(out, "w");

    if (sz >= hsz)
    {
        int16_t inc = 20;
        uchar cinc[2U] = {(inc & 0xFF00) >> 8U, inc & 0xFF};
        fseek(fs, 3216U, SEEK_SET);
        fwrite(cinc, sizeof(uchar), 2U, fs);

        uchar format = 1;
        fseek(fs, 3225U, SEEK_SET);
        fwrite(&format, sizeof(uchar), 1U, fs);

        uchar cns[2U] = {(ns & 0xFF00) >> 8U, ns & 0xFF};
        fseek(fs, 3220U, SEEK_SET);
        fwrite(cns, sizeof(uchar), 2U, fs);

        fseek(fs, 3600U, SEEK_SET);
        sz -= hsz;

        size_t allocSz = (maxBlock < sz ? maxBlock : sz);
        allocSz -= allocSz % dosz;
        size_t lnt = allocSz / dosz;

        uchar * buf = calloc(allocSz, sizeof(uchar));
        for (size_t i = 0; i < nt; i += lnt)
        {
            size_t chunk = (nt-i < lnt ? nt-i : lnt);
            #pragma omp parallel for
            for (size_t j = 0; j < chunk; j++)
            {
                int32_t il = ilNum(i+j);
                buf[dosz*j + 188] = (il >> 24) & 0xFF;
                buf[dosz*j + 189] = (il >> 16) & 0xFF;
                buf[dosz*j + 190] = (il >> 8)  & 0xFF;
                buf[dosz*j + 191] =  il        & 0xFF;

                int32_t xl = xlNum(i+j);
                buf[dosz*j + 192] = (xl >> 24) & 0xFF;
                buf[dosz*j + 193] = (xl >> 16) & 0xFF;
                buf[dosz*j + 194] = (xl >> 8)  & 0xFF;
                buf[dosz*j + 195] =  xl        & 0xFF;
            }
            fwrite(buf, sizeof(uchar), chunk*dosz, fs);
        }
        free(buf);
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
    makeSEGY("tmp/smallsegy.tmp", 261U, 400U, 1024U*1024U);
    makeSEGY("tmp/largesegy.tmp", 1000U, 2000000U, 1024U*1024U);
    return 0;
}

