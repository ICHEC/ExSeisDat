#ifndef SYS_SGLOBAL_INCLUDE_GUARD
#define SYS_SGLOBAL_INCLUDE_GUARD
#include "global.hh"

extern std::pair<size_t, size_t> decompose(size_t work, size_t nproc, size_t rank);
extern std::pair<size_t, size_t> blockDecomp(size_t work, size_t bsz, size_t nproc, size_t rank);
#endif
