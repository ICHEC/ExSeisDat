#include "wraptests.h"
#include "wraptesttools.hh"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "mockexseis.hh"
#include "mockparam.hh"
#include "mockrule.hh"

#include "exseiswraptest.hh"
#include "getminmaxwraptests.hh"
#include "paramwraptests.hh"
#include "readinterfacewraptests.hh"
#include "rulewraptests.hh"
#include "setwraptests.hh"
#include "writeinterfacewraptests.hh"

extern "C" {

void init_wraptests()
{
    // Initialize GoogleTest

    // Argc and argv.
    int argc = 1;

    // const char* needed for string literal, non-const char* needed for
    // google test.
    const char* program_name = "cwraptests";
    auto program_name_v      = std::vector<char>{
        program_name, program_name + strlen(program_name) + 1};
    char* program_name_ptr = program_name_v.data();

    // Throw so TestBuilder has something to catch and report.
    testing::GTEST_FLAG(throw_on_failure) = true;

    testing::InitGoogleTest(&argc, &program_name_ptr);

    // Disable GoogleTest printing exceptions.
    testing::TestEventListeners& listeners =
        testing::UnitTest::GetInstance()->listeners();
    listeners.Release(listeners.default_result_printer());

    // Setup and add the CheckReturnListener
    exseis::piol::check_return_listener() =
        new exseis::piol::CheckReturnListener;
    listeners.Append(exseis::piol::check_return_listener());

    //::testing::FLAGS_gmock_verbose = "info";
    ::testing::FLAGS_gmock_verbose = "error";

    // Add test initializers here
    testing::InSequence s;
    auto test_exseis = test_piol_exseis();
    auto test_rule   = test_piol_file_rule();
    auto test_param  = test_piol_file_trace_metadata(test_rule);
    test_piol_file_get_min_max(test_exseis, test_param);
    test_piol_file_read_interface(test_exseis, test_param);
    test_piol_file_write_interface(test_exseis, test_param);
    test_piol_set(test_exseis, test_param);

    // Add cleanup
    EXPECT_CALL(exseis::piol::mock_trace_metadata(), dtor(EqDeref(test_param)));
    EXPECT_CALL(exseis::piol::mock_rule(), Rule_dtor(testing::_));
    EXPECT_CALL(exseis::piol::mock_rule(), Rule_dtor(EqDeref(test_rule)));
    EXPECT_CALL(exseis::piol::mock_exseis(), dtor(EqDeref(test_exseis)));
}

void wraptest_ok()
{
    exseis::piol::return_checker().Call();
}

}  // extern "C"
