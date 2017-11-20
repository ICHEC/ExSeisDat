#include "wraptesttools.hh"

#include "piolstub/exseisstub.hh"

#include "exseiswraptest.hh"

#include "gtest/gtest.h"


void test_PIOL_ExSeis_new(std::function<void()> callback) {
    auto& PIOL_ExSeis_new_tester = make_test_builder(
        "PIOL::ExSeis::ExSeis(PIOL::Verbosity)",
        PIOL_ExSeis_new_cb,
        callback
    );

    // Test calls to ExSeis(verbosity) with the following args:
    std::pair<std::string, PIOL::Verbosity> exseis_new_args[] = {
        {"", PIOL_VERBOSITY_NONE},
        {"PIOL_VERBOSITY_NONE",     PIOL_VERBOSITY_NONE},
        {"PIOL_VERBOSITY_MINIMAL",  PIOL_VERBOSITY_MINIMAL},
        {"PIOL_VERBOSITY_EXTENDED", PIOL_VERBOSITY_EXTENDED},
        {"PIOL_VERBOSITY_VERBOSE",  PIOL_VERBOSITY_VERBOSE},
        {"PIOL_VERBOSITY_MAX",      PIOL_VERBOSITY_MAX},
    };

    for(auto args: exseis_new_args)
    {
        PIOL_ExSeis_new_tester.next_test(
            std::string{} + "Testing PIOL::ExSeis(" + args.first + ")",
            [args](PIOL::Verbosity verbosity) {
                EXPECT_EQ(verbosity, args.second);
            }
        );
    }

}

void test_PIOL_ExSeis_getRank(std::function<void()> callback) {
    auto& PIOL_ExSeis_getRank_tester = make_test_builder(
        "PIOL::ExSeis::getRank()",
        PIOL_ExSeis_getRank_cb,
        callback
    );

    PIOL_ExSeis_getRank_tester.next_test(
        "PIOL::ExSeis::getRank()",
        [] { return 0; }
    );
}

void test_PIOL_ExSeis_getNumRank(std::function<void()> callback) {
    auto& PIOL_ExSeis_getNumRank_tester = make_test_builder(
        "PIOL::ExSeis::getNumRank()",
        PIOL_ExSeis_getNumRank_cb,
        callback
    );

    PIOL_ExSeis_getNumRank_tester.next_test(
        "PIOL::ExSeis::getNumRank()",
        [] { return 10; }
    );
}

void test_PIOL_ExSeis_barrier(std::function<void()> callback) {
    auto& PIOL_ExSeis_barrier_tester = make_test_builder(
        "PIOL::ExSeis::barrier()",
        PIOL_ExSeis_barrier_cb,
        callback
    );

    PIOL_ExSeis_barrier_tester.next_test(
        "PIOL::ExSeis::barrier()",
        [] { /* do nothing */ }
    );
}

void test_PIOL_ExSeis_max(std::function<void()> callback) {
    auto& PIOL_ExSeis_max_tester = make_test_builder(
        "PIOL::ExSeis::max(size_t n)",
        PIOL_ExSeis_max_cb,
        callback
    );

    PIOL_ExSeis_max_tester.next_test(
        "PIOL::ExSeis::max(0)",
        [] (size_t n) {
            EXPECT_EQ(n, 0U);
            return 30;
        }
    );

    PIOL_ExSeis_max_tester.next_test(
        "PIOL::ExSeis::max(10)",
        [] (size_t n) {
            EXPECT_EQ(n, 40U);
            return 50;
        }
    );
}

void test_PIOL_ExSeis_isErr(std::function<void()> callback) {
    auto& PIOL_ExSeis_isErr_tester = make_test_builder(
        "PIOL::ExSeis::isErr(std::string msg)",
        PIOL_ExSeis_isErr_cb,
        callback
    );

    // Test call to isErr()
    PIOL_ExSeis_isErr_tester.next_test(
        "Testing PIOL::ExSeis::isErr()",
        [](std::string msg){
            EXPECT_TRUE(msg.empty());
        }
    );

    // Test call to isErr("Test isErr message")
    std::string test_message = "Test isErr message";
    PIOL_ExSeis_isErr_tester.next_test(
        std::string{} + "Testing PIOL::ExSeis::isErr(\"" + test_message + "\")",
        [=](std::string msg){
            EXPECT_EQ(msg, test_message);
        }
    );
}

void test_PIOL_ExSeis_delete(std::function<void()> callback) {
    auto& PIOL_ExSeis_delete_tester = make_test_builder(
        "PIOL::ExSeis::~ExSeis()",
        PIOL_ExSeis_delete_cb,
        callback
    );

    // Expect 5 calls to ~ExSeis();
    for(size_t i=0; i<5; i++) {
        PIOL_ExSeis_delete_tester.next_test(
            std::string{} + "Testing PIOL::ExSeis::~ExSeis()",
            [](){ /* empty callback */ }
        );
    }
}


void test_PIOL_ExSeis(std::function<void()> callback) {
    test_runner(
        test_PIOL_ExSeis_new,
        test_PIOL_ExSeis_getRank,
        test_PIOL_ExSeis_getNumRank,
        test_PIOL_ExSeis_barrier,
        test_PIOL_ExSeis_max,
        test_PIOL_ExSeis_isErr,
        test_PIOL_ExSeis_delete,
        [callback](auto cb) { cb(); callback(); }
    );
}
