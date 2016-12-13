#ifndef UTIL_SGLOBALH_INCLUDE_GUARD
#define UTIL_SGLOBALH_INCLUDE_GUARD
#define _POSIX_C_SOURCE 200809L //POSIX includes
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cfileapi.h"

#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)

typedef struct
{
    size_t start;
    size_t sz;
} Extent;

Extent decompose(size_t, size_t, size_t);
typedef void (* ModPrm)(size_t, size_t, CParam);
typedef void (* ModTrc)(size_t, size_t, float *);

extern char * copyString(char *);
#endif

