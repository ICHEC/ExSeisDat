#include "sglobal.h"
#include <assert.h>
Extent decompose(size_t sz, size_t nrank, size_t rank)
{
    assert(nrank > rank);
    size_t r = sz%nrank;
    size_t q = sz/nrank;
    size_t start = q * rank + MIN(rank, r);
    Extent extent = {.start=start, .end=MIN(sz - start, q + (rank < r))};
    return extent;
}

char * copyString(char * oldstr)
{
    size_t len = strlen(oldstr);
    char * newstr = malloc(len);
    assert(newstr);
    strncpy(newstr, oldstr, len);
    return newstr;
}
