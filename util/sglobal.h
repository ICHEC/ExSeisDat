#ifndef UTIL_SGLOBALH_INCLUDE_GUARD
#define UTIL_SGLOBALH_INCLUDE_GUARD

#define _POSIX_C_SOURCE 200809L  //POSIX includes

#include "cfileapi.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)

/*! A structure to record a loop starting iteraton and number of iterations
 * (sz).  Used in decomposition of problems.
 */
typedef struct {
    size_t start;  //!< The starting point for the extent
    size_t sz;     //!< The size of the extent
} Extent;

/*! Function which takes the size of a problem, the number of ranks to decompose
 *  the problem over and the particular local rank for the function call.
 *  @param[in] sz The size of the problem.
 *  @param[in] nrank The total number of ranks.
 *  @param[in] rank The rank during the function call, generally the rank of the
 *             calling MPI process.
 *  @return Return an extent, a starting point (generally for a 'for' loop) and
 *          the number of iterations.
 */
Extent decompose(const size_t sz, const size_t nrank, const size_t rank);

typedef void (*ModPrm)(
  size_t,
  size_t,
  PIOL_File_Param*);  //!< Typedef for function which modifies parameters

typedef void (*ModTrc)(
  size_t, size_t, float*);  //!< Typedef for function which modifies traces

/*! Function which copies a string onto newly allocated memory. The caller is
 *  responsible for deallocating the memory with free. The newly allocated
 *  memory is equal to the length of the old string.
 *  @param[in] oldstr The old string to be copied to the new memory.
 *  @return The newly allocated string which contains the contents of oldstr
 */
char* copyString(const char* oldstr);

#endif
