#include <stddef.h>
#include "ctest.h"
typedef struct
{
    size_t start;
    size_t end;
} Extent;

Extent decompose(size_t sz, size_t nrank, size_t rank);


