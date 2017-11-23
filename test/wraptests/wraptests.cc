#include "wraptests.h"
#include "wraptesttools.hh"
#include "gtest/gtest.h"

#include "exseiswraptest.hh"
#include "readdirectwraptest.hh"

extern "C" {

bool all_tests_run;

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


    // Add test initializers here
    all_tests_run = false;
    test_runner(
        test_PIOL_ExSeis,
        test_PIOL_File_ReadDirect,
        [](auto cb) {
            all_tests_run = true;
            cb();
        }
    );
}

int finalize_wraptests() {
    if(all_tests_run) {
        std::cout
            << std::endl
            << "=" << std::endl
            << "= All tests run!" << std::endl
            << "=" << std::endl
            << std::endl;

        std::cout << "Testing Passed." << std::endl;
        return EXIT_SUCCESS;
    } else {
        std::cout
            << std::endl
            << "=" << std::endl
            << "= Some tests were not run!" << std::endl
            << "=" << std::endl
            << std::endl
            << "Expecting: " << current_test_name << std::endl
            << std::endl;

        std::cout << "Testing Failed." << std::endl;
        return EXIT_FAILURE;
    }
}

void fail_wraptests() {
    std::cout
        << "fail_wraptests called after test: " << previous_test_name
        << std::endl
        << "Expected value: " << expected_value
        << std::endl;
}

}
