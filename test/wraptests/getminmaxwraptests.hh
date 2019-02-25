#ifndef EXSEISDAT_TEST_WRAPTESTS_GETMINMAXWRAPTESTS_HH
#define EXSEISDAT_TEST_WRAPTESTS_GETMINMAXWRAPTESTS_HH

#include "mockexseis.hh"
#include "mockparam.hh"
#include <memory>

void test_piol_file_get_min_max(
    std::shared_ptr<exseis::piol::ExSeis*> piol,
    std::shared_ptr<exseis::piol::Trace_metadata*> param);

#endif  // EXSEISDAT_TEST_WRAPTESTS_GETMINMAXWRAPTESTS_HH
