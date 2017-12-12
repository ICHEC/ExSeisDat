#include "exseiswraptest.hh"

#include "mockexseis.hh"
#include "wraptesttools.hh"
#include "gtest/gtest.h"

#include <utility>


using namespace ::PIOL;
using namespace ::testing;

std::shared_ptr<PIOL::ExSeis*> test_PIOL_ExSeis()
{
    auto exseis_ptr = std::make_shared<ExSeis*>();
    EXPECT_CALL(mockExSeis(), ctor(_, PIOL_VERBOSITY_NONE))
        .WillOnce(SaveArg<0>(exseis_ptr));

    std::pair<std::string, Verbosity> exseis_new_args[] = {
        {"PIOL_VERBOSITY_NONE",     PIOL_VERBOSITY_NONE},
        {"PIOL_VERBOSITY_MINIMAL",  PIOL_VERBOSITY_MINIMAL},
        {"PIOL_VERBOSITY_EXTENDED", PIOL_VERBOSITY_EXTENDED},
        {"PIOL_VERBOSITY_VERBOSE",  PIOL_VERBOSITY_VERBOSE},
        {"PIOL_VERBOSITY_MAX",      PIOL_VERBOSITY_MAX},
    };

    for(auto args: exseis_new_args)
    {
        EXPECT_CALL(mockExSeis(), ctor(_, args.second));
    }

    EXPECT_CALL(mockExSeis(), getRank(_)).WillOnce(Return(0));
    EXPECT_CALL(returnChecker(), Call());

    EXPECT_CALL(mockExSeis(), getNumRank(_)).WillOnce(Return(10));
    EXPECT_CALL(returnChecker(), Call());

    EXPECT_CALL(mockExSeis(), barrier(_));

    EXPECT_CALL(mockExSeis(), max(_, 0)).WillOnce(Return(30));
    EXPECT_CALL(returnChecker(), Call());
    EXPECT_CALL(mockExSeis(), max(_, 40)).WillOnce(Return(50));
    EXPECT_CALL(returnChecker(), Call());

    EXPECT_CALL(mockExSeis(), isErr(_, ""));
    EXPECT_CALL(mockExSeis(), isErr(_, "Test isErr message"));

    for(size_t i=0; i<5; i++) {
        EXPECT_CALL(mockExSeis(), dtor(_));
    }

    return exseis_ptr;
}
