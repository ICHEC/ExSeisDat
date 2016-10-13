#ifndef UTIL_SGLOBALHH_INCLUDE_GUARD
#define UTIL_SGLOBALHH_INCLUDE_GUARD
#define _POSIX_C_SOURCE 200809L //POSIX includes
#include <unistd.h>
#include <utility>
#include "global.hh"

extern std::pair<size_t, size_t> decompose(size_t work, size_t nproc, size_t rank);
extern std::pair<size_t, size_t> blockDecomp(size_t sz, size_t bsz, size_t numRank, size_t rank, size_t off = 0);
extern std::vector<size_t> lobdecompose(PIOL::ExSeisPIOL * piol, size_t work, size_t numRank, size_t rank);

#endif
