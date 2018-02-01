#include <arpa/inet.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uchar;

void writePattern(FILE* fs, size_t sz, const uchar* pattern, size_t psz)
{
    size_t q = sz / psz;
    size_t r = sz % psz;
    for (size_t i = 0; i < q; i++)
        fwrite(pattern, sizeof(uchar), psz, fs);
    if (r) fwrite(pattern, sizeof(uchar), r, fs);
}

void makeFile(const char* name, size_t sz, const uchar* pattern, size_t psz)
{
    FILE* fs = fopen(name, "w");
    if (sz == 0) {
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

int32_t xNum(size_t i)
{
    return 1000L + (i / 2000L);
}

int32_t yNum(size_t i)
{
    return 1000L + (i % 2000L);
}

void makeSEGY(const char* out, size_t ns, size_t nt, size_t maxBlock)
{
    const size_t hsz  = 3600;
    const size_t thsz = 240U;
    size_t dosz       = (thsz + ns * sizeof(float));
    size_t sz         = hsz + (thsz + ns * sizeof(float)) * nt;
    FILE* fs          = fopen(out, "w");
    //If you are trapped here, make sure you created the required folders
    assert(
      fs);
    if (sz >= hsz) {
        int16_t inc    = 20;
        uchar cinc[2U] = {(inc & 0xFF00) >> 8U, inc & 0xFF};
        fseek(fs, 3216U, SEEK_SET);
        fwrite(cinc, sizeof(uchar), 2U, fs);

        uchar format = 5;
        fseek(fs, 3225U, SEEK_SET);
        fwrite(&format, sizeof(uchar), 1U, fs);

        uchar cns[2U] = {(ns & 0xFF00) >> 8U, ns & 0xFF};
        fseek(fs, 3220U, SEEK_SET);
        fwrite(cns, sizeof(uchar), 2U, fs);

        fseek(fs, 3600U, SEEK_SET);
        uchar zero = 0;
        fwrite(&zero, 1, 1, fs);
        fseek(fs, 3600U, SEEK_SET);
        sz -= hsz;

        size_t allocSz = (maxBlock < sz ? maxBlock : sz);
        allocSz -= allocSz % dosz;
        size_t lnt = allocSz / dosz;

        uchar* buf = calloc(allocSz, sizeof(uchar));
        for (size_t i = 0; i < nt; i += lnt) {
            size_t chunk = (nt - i < lnt ? nt - i : lnt);
            #pragma omp parallel for
            for (size_t j = 0; j < chunk; j++) {
                for (size_t k = 0; k < ns; k++) {
                    union {
                        float f;
                        uint32_t i;
                    } n                             = {.f = (i + j + k)};
                    buf[dosz * j + 240 + 4 * k + 0] = n.i >> 24 & 0xFF;
                    buf[dosz * j + 240 + 4 * k + 1] = n.i >> 16 & 0xFF;
                    buf[dosz * j + 240 + 4 * k + 2] = n.i >> 8 & 0xFF;
                    buf[dosz * j + 240 + 4 * k + 3] = n.i & 0xFF;
                }

                int16_t scale      = 1;
                buf[dosz * j + 70] = scale >> 8 & 0xFF;
                buf[dosz * j + 71] = scale & 0xFF;

                int32_t x          = xNum(i + j);
                buf[dosz * j + 72] = x >> 24 & 0xFF;
                buf[dosz * j + 73] = x >> 16 & 0xFF;
                buf[dosz * j + 74] = x >> 8 & 0xFF;
                buf[dosz * j + 75] = x & 0xFF;

                int32_t y          = yNum(i + j);
                buf[dosz * j + 76] = y >> 24 & 0xFF;
                buf[dosz * j + 77] = y >> 16 & 0xFF;
                buf[dosz * j + 78] = y >> 8 & 0xFF;
                buf[dosz * j + 79] = y & 0xFF;

                int32_t il          = ilNum(i + j);
                buf[dosz * j + 188] = il >> 24 & 0xFF;
                buf[dosz * j + 189] = il >> 16 & 0xFF;
                buf[dosz * j + 190] = il >> 8 & 0xFF;
                buf[dosz * j + 191] = il & 0xFF;

                int32_t xl          = xlNum(i + j);
                buf[dosz * j + 192] = xl >> 24 & 0xFF;
                buf[dosz * j + 193] = xl >> 16 & 0xFF;
                buf[dosz * j + 194] = xl >> 8 & 0xFF;
                buf[dosz * j + 195] = xl & 0xFF;
            }
            fwrite(buf, sizeof(uchar), chunk * dosz, fs);
        }
        free(buf);
        fclose(fs);
    }
}

int main(int argc, char* argv[])
{
#define PSZ_VALUE (0x100)
    const size_t psz = PSZ_VALUE;
    uchar pattern[PSZ_VALUE];
    for (size_t i = 0; i < psz; i++) {
        pattern[i] = i % psz;
        //pattern[i] = i + i % 3 + i % 9 + i % (psz - 7);
    }

    for (int i = 1; i < argc; i++) {

        //
        // Files for unit tests
        //

        printf("Generating file: %s\n", argv[i]);

        if (strcmp("smallFilePattern.tmp", argv[i]) == 0) {
            makeFile(argv[i], 4096ll, pattern, psz);
            continue;
        }
        if (strcmp("largeFilePattern.tmp", argv[i]) == 0) {
            makeFile(argv[i], 10ll * 1024ll * 1024ll * 1024ll, pattern, psz);
            continue;
        }
        if (strcmp("smallsegy.tmp", argv[i]) == 0) {
            makeSEGY(argv[i], 261U, 400U, 1024U * 1024U);
            continue;
        }
        if (strcmp("bigtracesegy.tmp", argv[i]) == 0) {
            makeSEGY(argv[i], 32000U, 40000U, 1024U * 1024U);
            continue;
        }
        if (strcmp("largesegy.tmp", argv[i]) == 0) {
            makeSEGY(argv[i], 1000U, 2000000U, 1024U * 1024U);
            continue;
        }


        //
        // Files for system tests
        //

        if (strcmp("notrace.segy", argv[i]) == 0) {
            makeSEGY(argv[i], 1000U, 0U, 3600U);
            continue;
        }
        if (strcmp("onetrace.segy", argv[i]) == 0) {
            makeSEGY(argv[i], 1000U, 1U, 1024U * 1024U * 1024U);
            continue;
        }
        if (strcmp("onebigtrace.segy", argv[i]) == 0) {
            makeSEGY(argv[i], 32768U, 1U, 16U * 1024U * 1024U);
            continue;
        }
        if (strcmp("smallsegy.segy", argv[i]) == 0) {
            makeSEGY(argv[i], 1000U, 2200000U, 1024U * 1024U * 1024U);
            continue;
        }


        // Uncaught value for argv
        printf("Error! Unknown filename %s!\n", argv[i]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
