#ifndef EXSEISDAT_TEST_WRAPTESTS_PARAMWRAPTESTS_HH
#define EXSEISDAT_TEST_WRAPTESTS_PARAMWRAPTESTS_HH

#include "mockparam.hh"
#include "mockrule.hh"

#include "googletest_variable_instances.hh"

std::shared_ptr<exseis::piol::Trace_metadata*> test_piol_file_trace_metadata(
  std::shared_ptr<exseis::piol::Rule*> test_rule);

#endif  // EXSEISDAT_TEST_WRAPTESTS_PARAMWRAPTESTS_HH
