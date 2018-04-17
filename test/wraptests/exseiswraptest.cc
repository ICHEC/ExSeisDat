#include "exseiswraptest.hh"

#include "mockexseis.hh"
#include "wraptesttools.hh"

#include "gtest/gtest.h"

#include <utility>


using namespace testing;
using namespace exseis::PIOL;

std::shared_ptr<ExSeis*> test_PIOL_ExSeis()
{
    auto exseis_ptr = std::make_shared<ExSeis*>();
    EXPECT_CALL(mockExSeis(), ctor(_, PIOL_VERBOSITY_NONE, _))
      .WillOnce(SaveArg<0>(exseis_ptr));

    const Verbosity verbosities[] = {
      PIOL_VERBOSITY_NONE, PIOL_VERBOSITY_MINIMAL, PIOL_VERBOSITY_EXTENDED,
      PIOL_VERBOSITY_VERBOSE, PIOL_VERBOSITY_MAX};

    for (auto verbosity : verbosities) {
        EXPECT_CALL(mockExSeis(), ctor(_, verbosity, _));
    }

    EXPECT_CALL(mockExSeis(), getRank(_)).WillOnce(CheckReturn(0));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mockExSeis(), getNumRank(_)).WillOnce(CheckReturn(10));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mockExSeis(), barrier(_));

    EXPECT_CALL(mockExSeis(), max(_, 0)).WillOnce(CheckReturn(30));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());
    EXPECT_CALL(mockExSeis(), max(_, 40)).WillOnce(CheckReturn(50));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mockExSeis(), isErr(_, ""));
    EXPECT_CALL(mockExSeis(), isErr(_, "Test isErr message"));

    for (size_t i = 0; i < 5; i++) {
        EXPECT_CALL(mockExSeis(), dtor(_));
    }

    return exseis_ptr;
}
