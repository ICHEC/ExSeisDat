#include <stdio.h>
typedef long long int llint;
typedef unsigned char uchar;
void makeFile(const char * name, const size_t sz, const uchar * pattern, const size_t psz)
{
    static const char zero = '\0';
    FILE * fs = fopen(name, "w");

    if (sz == 0)
    {
        fclose(fs);
        return;
    }

    llint q = sz / psz;
    llint r = sz % psz;

    fseek(fs, 0U, SEEK_SET);
    for (llint i = 0; i < q; i++)
        fwrite(pattern, sizeof(uchar), psz, fs);
    fwrite(pattern, sizeof(uchar), r, fs);
    fclose(fs);
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
    return 0;
}

