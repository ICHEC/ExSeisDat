#include "exseiswraptest.hh"

#include "mockexseis.hh"
#include "wraptesttools.hh"

#include "gtest/gtest.h"

#include <utility>


using namespace testing;
using namespace exseis::utils;
using namespace exseis::piol;

std::shared_ptr<ExSeis*> test_piol_exseis()
{
    auto exseis_ptr = std::make_shared<ExSeis*>();
    EXPECT_CALL(mock_exseis(), ctor(_, exseis::utils::Verbosity::none, _))
        .WillOnce(SaveArg<0>(exseis_ptr));

    const Verbosity verbosities[] = {
        exseis::utils::Verbosity::none, exseis::utils::Verbosity::minimal,
        exseis::utils::Verbosity::extended, exseis::utils::Verbosity::verbose,
        exseis::utils::Verbosity::max};

    for (auto verbosity : verbosities) {
        EXPECT_CALL(mock_exseis(), ctor(_, verbosity, _));
    }

    EXPECT_CALL(mock_exseis(), get_rank(_)).WillOnce(CheckReturn(0));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mock_exseis(), get_num_rank(_)).WillOnce(CheckReturn(10));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mock_exseis(), barrier(_));

    EXPECT_CALL(mock_exseis(), max(_, 0)).WillOnce(CheckReturn(30));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());
    EXPECT_CALL(mock_exseis(), max(_, 40)).WillOnce(CheckReturn(50));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mock_exseis(), assert_ok(_, ""));
    EXPECT_CALL(mock_exseis(), assert_ok(_, "Test assert_ok message"));

    for (size_t i = 0; i < 5; i++) {
        EXPECT_CALL(mock_exseis(), dtor(_));
    }

    return exseis_ptr;
}
