#ifndef PIOLWRAPTESTSEXSEISWRAPTEST_HEADER_GUARD
#define PIOLWRAPTESTSEXSEISWRAPTEST_HEADER_GUARD

#include <memory>
#include "mockexseis.hh"

//void test_PIOL_ExSeis(std::function<void()> callback);
std::shared_ptr<PIOL::ExSeis*> test_PIOL_ExSeis();

#endif
