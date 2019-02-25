#ifndef EXSEISDAT_TEST_WRAPTESTS_SETWRAPTESTS_HH
#define EXSEISDAT_TEST_WRAPTESTS_SETWRAPTESTS_HH

#include "mockexseis.hh"

#include <memory>

void test_piol_set(
    std::shared_ptr<exseis::piol::ExSeis*> piol,
    std::shared_ptr<exseis::piol::Trace_metadata*> trace_metadata);

#endif  // EXSEISDAT_TEST_WRAPTESTS_SETWRAPTESTS_HH
