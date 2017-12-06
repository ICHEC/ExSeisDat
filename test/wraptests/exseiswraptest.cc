#include "exseiswraptest.hh"

#include "mockexseis.hh"
#include "wraptesttools.hh"
#include "gtest/gtest.h"


using namespace ::PIOL;
using namespace ::testing;

void test_PIOL_ExSeis()
{
    std::pair<std::string, Verbosity> exseis_new_args[] = {
        {"", PIOL_VERBOSITY_NONE},
        {"PIOL_VERBOSITY_NONE",     PIOL_VERBOSITY_NONE},
        {"PIOL_VERBOSITY_MINIMAL",  PIOL_VERBOSITY_MINIMAL},
        {"PIOL_VERBOSITY_EXTENDED", PIOL_VERBOSITY_EXTENDED},
        {"PIOL_VERBOSITY_VERBOSE",  PIOL_VERBOSITY_VERBOSE},
        {"PIOL_VERBOSITY_MAX",      PIOL_VERBOSITY_MAX},
    };

    for(auto args: exseis_new_args)
    {
        EXPECT_CALL(mockExSeis(), ctor(args.second));
    }

    EXPECT_CALL(mockExSeis(), getRank()).WillOnce(Return(0));
    EXPECT_CALL(returnChecker(), Call());

    EXPECT_CALL(mockExSeis(), getNumRank()).WillOnce(Return(10));
    EXPECT_CALL(returnChecker(), Call());

    EXPECT_CALL(mockExSeis(), barrier());

    EXPECT_CALL(mockExSeis(), max(0)).WillOnce(Return(30));
    EXPECT_CALL(returnChecker(), Call());
    EXPECT_CALL(mockExSeis(), max(40)).WillOnce(Return(50));
    EXPECT_CALL(returnChecker(), Call());

    EXPECT_CALL(mockExSeis(), isErr(""));
    EXPECT_CALL(mockExSeis(), isErr("Test isErr message"));

    for(size_t i=0; i<5; i++) {
        EXPECT_CALL(mockExSeis(), dtor());
    }
}
