#ifndef EXSEISDAT_TEST_WRAPTESTS_WRITEINTERFACEWRAPTESTS_HH
#define EXSEISDAT_TEST_WRAPTESTS_WRITEINTERFACEWRAPTESTS_HH

#include "mockexseis.hh"
#include "mockparam.hh"

#include <memory>

void test_piol_file_write_interface(
  std::shared_ptr<exseis::piol::ExSeis*> piol,
  std::shared_ptr<exseis::piol::Trace_metadata*> param);

#endif  // EXSEISDAT_TEST_WRAPTESTS_WRITEINTERFACEWRAPTESTS_HH
