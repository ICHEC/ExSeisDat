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
#include "readdirectwraptests.hh"
#include "rulewraptests.hh"
#include "setwraptests.hh"
#include "writedirectwraptests.hh"

extern "C" {

void init_wraptests()
{
    // Initialize GoogleTest

    // Argc and argv.
    int argc = 1;

    // const char* needed for string literal, non-const char* needed for
    // google test.
    const char* program_name = "cwraptests";
    auto program_name_v =
      std::vector<char>{program_name, program_name + strlen(program_name) + 1};
    char* program_name_ptr = program_name_v.data();

    // Throw so TestBuilder has something to catch and report.
    testing::GTEST_FLAG(throw_on_failure) = true;

    testing::InitGoogleTest(&argc, &program_name_ptr);

    // Disable GoogleTest printing exceptions.
    testing::TestEventListeners& listeners =
      testing::UnitTest::GetInstance()->listeners();
    listeners.Release(listeners.default_result_printer());

    // Setup and add the CheckReturnListener
    exseis::PIOL::checkReturnListener() = new exseis::PIOL::CheckReturnListener;
    listeners.Append(exseis::PIOL::checkReturnListener());

    //::testing::FLAGS_gmock_verbose = "info";
    ::testing::FLAGS_gmock_verbose = "error";

    // Add test initializers here
    testing::InSequence s;
    auto test_exseis = test_PIOL_ExSeis();
    auto test_rule   = test_PIOL_File_Rule();
    auto test_param  = test_PIOL_File_Param(test_rule);
    test_PIOL_File_getMinMax(test_exseis, test_param);
    test_PIOL_File_ReadDirect(test_exseis, test_param);
    test_PIOL_File_WriteDirect(test_exseis, test_param);
    test_PIOL_Set(test_exseis);

    // Add cleanup
    EXPECT_CALL(exseis::PIOL::mockParam(), dtor(EqDeref(test_param)));
    EXPECT_CALL(exseis::PIOL::mockRule(), dtor(EqDeref(test_rule)));
    EXPECT_CALL(exseis::PIOL::mockExSeis(), dtor(EqDeref(test_exseis)));
}

void wraptest_ok()
{
    exseis::PIOL::returnChecker().Call();
}

}  // extern "C"
