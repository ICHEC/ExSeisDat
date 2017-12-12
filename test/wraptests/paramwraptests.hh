#ifndef PIOLWRAPTESTSPARAMWRAPTEST_HEADER_GUARD
#define PIOLWRAPTESTSPARAMWRAPTEST_HEADER_GUARD

#include "mockparam.hh"
#include "mockrule.hh"

std::shared_ptr<PIOL::File::Param*> test_PIOL_File_Param(
    std::shared_ptr<PIOL::File::Rule*> test_rule
);

#endif
