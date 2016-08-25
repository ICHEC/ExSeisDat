#ifndef SYS_SGLOBAL_INCLUDE_GUARD
#define SYS_SGLOBAL_INCLUDE_GUARD
#include "global.hh"

extern std::pair<size_t, size_t> decompose(size_t work, size_t nproc, size_t rank);
extern std::pair<size_t, size_t> blockDecomp(size_t sz, size_t bsz, size_t numRank, size_t rank, size_t off = 0);
#endif
