#ifndef UTIL_SGLOBALH_INCLUDE_GUARD
#define UTIL_SGLOBALH_INCLUDE_GUARD
#define _POSIX_C_SOURCE 200809L //POSIX includes
#include <stddef.h>
#include "ctest.h"
#include "cfileapi.h"
typedef struct
{
    size_t start;
    size_t end;
} Extent;

Extent decompose(size_t, size_t, size_t);
typedef void (* ModPrm)(size_t, TraceParam *);
typedef void (* ModTrc)(size_t, size_t, float *);

extern char * copyString(char *);
#endif

