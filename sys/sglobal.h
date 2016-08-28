#include <stddef.h>
#include "ctest.h"
#include "cfileapi.h"
typedef struct
{
    size_t start;
    size_t end;
} Extent;

Extent decompose(size_t sz, size_t nrank, size_t rank);
typedef void (* ModPrm)(size_t, TraceParam *);
typedef void (* ModTrc)(size_t, size_t, float *);


