#ifndef PIOLWRAPTESTSEXSEISWRAPTEST_HEADER_GUARD
#define PIOLWRAPTESTSEXSEISWRAPTEST_HEADER_GUARD

#include "mockexseis.hh"
#include <memory>

//void test_PIOL_ExSeis(std::function<void()> callback);
std::shared_ptr<PIOL::ExSeis*> test_PIOL_ExSeis();

#endif  // PIOLWRAPTESTSEXSEISWRAPTEST_HEADER_GUARD
