#include "wraptests.h"
#include "wraptesttools.hh"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "exseiswraptest.hh"
#include "rulewraptests.hh"
#include "paramwraptests.hh"
#include "getminmaxwraptests.hh"
#include "readdirectwraptests.hh"

extern "C" {

void init_wraptests()
{
    // Initialize GoogleTest

    // Argc and argv.
    int argc = 1;
    char* argv = NULL;

    // const char* needed for string literal, non-const char* needed for
    // google test.
    const char* cargv = "cwraptests";
    argv = new char[strlen(cargv)+1];
    strcpy(argv, cargv);

    // Throw so TestBuilder has something to catch and report.
    testing::GTEST_FLAG(throw_on_failure) = true;
    testing::InitGoogleTest(&argc, &argv);

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
}

void wraptest_ok()
{
    PIOL::returnChecker().Call();
}

}
