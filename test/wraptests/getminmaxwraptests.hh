#ifndef PIOLWRAPTESTSGETMINMAX_INCLUDE_GUARD
#define PIOLWRAPTESTSGETMINMAX_INCLUDE_GUARD

#include "mockexseis.hh"
#include "mockparam.hh"
#include <memory>

void test_PIOL_File_getMinMax(
  std::shared_ptr<exseis::PIOL::ExSeis*> piol,
  std::shared_ptr<exseis::PIOL::Param*> param);

#endif  // PIOLWRAPTESTSGETMINMAX_INCLUDE_GUARD
