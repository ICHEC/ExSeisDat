#ifndef PIOLWRAPTESTSWRITEDIRECTWRAPTESTS_INCLUDE_GUARD
#define PIOLWRAPTESTSWRITEDIRECTWRAPTESTS_INCLUDE_GUARD

#include "mockexseis.hh"
#include "mockparam.hh"

#include <memory>

void test_PIOL_File_WriteDirect(
  std::shared_ptr<exseis::PIOL::ExSeis*> piol,
  std::shared_ptr<exseis::PIOL::Param*> param);

#endif  // PIOLWRAPTESTSWRITEDIRECTWRAPTESTS_INCLUDE_GUARD
