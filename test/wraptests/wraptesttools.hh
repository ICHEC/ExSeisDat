#ifndef PIOLWRAPTESTSWRAPTESTTOOLS_HEADER_GUARD
#define PIOLWRAPTESTSWRAPTESTTOOLS_HEADER_GUARD

#include "gmock/gmock.h"

testing::MockFunction<void()>& returnChecker();

extern std::string checking_return;


class CheckReturnListener: public testing::EmptyTestEventListener
{
public:
    CheckReturnListener();

    // Called after EXPECT_CALL throws
    virtual void OnTestPartResult(
        const ::testing::TestPartResult& test_part_result);

    // Call to set the awaiting_check_ flag, and the return_value_ string.
    // This will set the listener to report the return_value on failure.
    static void wait_for_check(std::string return_value);

    // Call when the return value has been verified. This resets the
    // awaiting_check_ flag to false;
    static void check_successful();

private:
    static bool awaiting_check_;
    static std::string return_value_;
};

#endif
