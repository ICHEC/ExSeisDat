#include "readdirectwraptest.hh"

#include "piolstub/readdirectstub.hh"
#include "wraptesttools.hh"
#include "gtest/gtest.h"


void test_PIOL_File_ReadDirect_new(std::function<void()> callback) {
    auto& PIOL_File_ReadDirect_new_tester = make_test_builder(
        "PIOL::File::ReadDirect(PIOL::Piol piol, std::string filename)",
        PIOL_File_ReadDirect_new_cb,
        callback
    );

    std::string test_filename = "test_readdirect_filename.sgy";
    PIOL_File_ReadDirect_new_tester.next_test(
        std::string{} + "PIOL::File::ReadDirect(piol, \"" + test_filename + "\")",
        [=](PIOL::Piol piol, std::string filename) {
            EXPECT_EQ(filename, test_filename);
        }
    );
}

void test_PIOL_File_ReadDirect(std::function<void()> callback) {
    test_runner(
        test_PIOL_File_ReadDirect_new,
        [=](auto cb) { cb(); callback(); }
    );
}
