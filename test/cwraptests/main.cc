#include <iostream>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "setstub.hh"

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    int code = RUN_ALL_TESTS();

    return code;
}


TEST(PIOL_Set,new) {
    bool ran = false;
    PIOL_Set_new_1_cb = [&ran](
        PIOL::Piol piol_, std::string pattern, std::string outfix_,
        std::shared_ptr<PIOL::File::Rule> rule_
    ) {
        ran = true;
    };

    PIOL_ExSeisHandle piol = PIOL_ExSeis_new();
    PIOL_SetHandle set = PIOL_Set_new(piol, "");
    EXPECT_EQ(ran, true);
}
