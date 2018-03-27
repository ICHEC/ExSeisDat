#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    assert(argc > 1);
    FILE* fs = fopen(argv[1], "r");
    unsigned char cns[2U];
    fseek(fs, 3220U, SEEK_SET);
    fread(cns, sizeof(char), 2U, fs);
    int64_t ns = cns[0] << 8 | cns[1];
    fprintf(stderr, "int16_t ns = %" PRId64 "\n", ns);

    fseek(fs, 0U, SEEK_END);

    int64_t fsz = ftell(fs);
    fprintf(stderr, "uint64_t fsz = %" PRId64 "\n", fsz);
    size_t nt = (fsz - 3600U) / (sizeof(float) * ns + 240U);
    assert(!((fsz - 3600U) % (sizeof(float) * ns + 240U)));

    fseek(fs, 3840U, SEEK_SET);

    size_t rnt       = (nt > 100 ? 100 : nt);
    uint32_t* traces = malloc(ns * sizeof(uint32_t));
    assert(traces);
    printf("#include <vector>\n#include <stdint.h>\n");
    printf("std::vector<uint32_t> rawTraces = {");
    for (size_t i = 0; i < rnt; i++) {
        fread(traces, sizeof(uint32_t), ns, fs);
        fseek(fs, 240U, SEEK_CUR);
        printf("    0x%x, ", traces[0]);
        for (int64_t j = 1U; j < ns - 1U; j++)
            printf("0x%x, ", traces[j]);
        if (i == rnt - 1)
            printf("0x%x\n", traces[ns - 1]);
        else
            printf("0x%x,\n", traces[ns - 1]);
    }
    printf("};\n");
    fclose(fs);
}
