#ifndef PIOLCTEST_INCLUDE_GUARD
#define PIOLCTEST_INCLUDE_GUARD
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void CTest_quit(const char * msg, size_t line);
#define MIN(x, y) (x < y ? x : y)
#define CMP(X, Y) { size_t line = __LINE__; \
                      if (X != Y) \
                      { \
                        CTest_quit("Failure in ## __FILE__ ## Line %zu\n", line);\
                      }}

#define CMP_STR(X, Y) { size_t line = __LINE__; \
                      size_t len1 = strlen(X), len2 = strlen(Y); \
                      if (strncmp(X, Y, MIN(len1, len2))) \
                      { \
                          CTest_quit("Failure in ## __FILE__ ## Line %zu\n", line);\
                      }}
#endif

