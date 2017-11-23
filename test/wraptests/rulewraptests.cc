#include "wraptesttools.hh"
#include "rulewraptests.hh"
#include "piolstub/rulestub.hh"


void test_PIOL_File_Rule_new(std::function<void()> callback) {
    auto& PIOL_File_Rule_new_tester = make_test_builder(
        "PIOL::File::Rule::Rule(bool full, bool defaults, bool extras)",
        PIOL_File_Rule_new_2_cb,
        callback
    );

    PIOL_File_Rule_new_tester.next_test(
        "PIOL::File::Rule::Rule(true, true, false)",
        [](bool full, bool defaults, bool extras) {
            EXPECT_EQ(full,     true);
            EXPECT_EQ(defaults, true);
            EXPECT_EQ(extras,   false);
        }
    );

    PIOL_File_Rule_new_tester.next_test(
        "PIOL::File::Rule::Rule(true, false, false)",
        [](bool full, bool defaults, bool extras) {
            EXPECT_EQ(full,     true);
            EXPECT_EQ(defaults, false);
            EXPECT_EQ(extras,   false);
        }
    );
}

// void test_PIOL_File_Rule_addLong(std::function<void()> callback) {
//     auto& PIOL_File_Rule_addLong_tester = make_test_builder(
//         "PIOL::File::Rule::addLong(",
//         [](
// }

void test_PIOL_File_Rule(std::function<void()> callback) {
    test_runner(
        test_PIOL_File_Rule_new,
        [=](auto cb) { cb(); callback(); }
    );
}
