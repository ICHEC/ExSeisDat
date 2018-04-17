#ifndef PIOLWRAPTESTSPARAMWRAPTEST_HEADER_GUARD
#define PIOLWRAPTESTSPARAMWRAPTEST_HEADER_GUARD

#include "mockparam.hh"
#include "mockrule.hh"

#include "googletest_variable_instances.hh"

std::shared_ptr<exseis::PIOL::Param*> test_PIOL_File_Param(
  std::shared_ptr<exseis::PIOL::Rule*> test_rule);

#endif
