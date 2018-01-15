#include "wraptests.h"
#include "wraptesttools.hh"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "mockparam.hh"
#include "mockrule.hh"
#include "mockexseis.hh"

#include "exseiswraptest.hh"
#include "rulewraptests.hh"
#include "paramwraptests.hh"
#include "getminmaxwraptests.hh"
#include "readdirectwraptests.hh"
#include "writedirectwraptests.hh"
#include "setwraptests.hh"

extern "C" {

void init_wraptests()
{
    // Initialize GoogleTest

    // Argc and argv.
    int argc = 1;

    // const char* needed for string literal, non-const char* needed for
    // google test.
    const char * program_name = "cwraptests";
    auto program_name_v = std::vector<char>{
        program_name, program_name+strlen(program_name)+1
    };
    char * program_name_ptr = program_name_v.data();

    // Throw so TestBuilder has something to catch and report.
    testing::GTEST_FLAG(throw_on_failure) = true;

    testing::InitGoogleTest(&argc, &program_name_ptr);

    // Disable GoogleTest printing exceptions.
    testing::TestEventListeners& listeners =
        testing::UnitTest::GetInstance()->listeners();
    listeners.Release(listeners.default_result_printer());

    // Setup and add the CheckReturnListener
    PIOL::checkReturnListener() = new PIOL::CheckReturnListener;
    listeners.Append(PIOL::checkReturnListener());

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
    EXPECT_CALL(PIOL::File::mockParam(), dtor(EqDeref(test_param)));
    EXPECT_CALL(PIOL::File::mockRule(),  dtor(EqDeref(test_rule)));
    EXPECT_CALL(PIOL::mockExSeis(),      dtor(EqDeref(test_exseis)));
}

void wraptest_ok()
{
    PIOL::returnChecker().Call();
}

}
