#ifndef EXSEISDAT_TEST_WRAPTESTS_READINTERFACEWRAPTESTS_HH
#define EXSEISDAT_TEST_WRAPTESTS_READINTERFACEWRAPTESTS_HH

#include "mockexseis.hh"
#include "mockparam.hh"

#include <memory>

void test_piol_file_read_interface(
  std::shared_ptr<exseis::piol::ExSeis*> piol,
  std::shared_ptr<exseis::piol::Trace_metadata*> param);

#endif  // EXSEISDAT_TEST_WRAPTESTS_READINTERFACEWRAPTESTS_HH
