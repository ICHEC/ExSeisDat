#include "wraptesttools.hh"

testing::MockFunction<void()>& returnChecker()
{
    static testing::MockFunction<void()> returnChecker;
    return returnChecker;
}



CheckReturnListener::CheckReturnListener()
{
    CheckReturnListener::awaiting_check_ = false;
    CheckReturnListener::return_value_   = std::string{};
}

void CheckReturnListener::OnTestPartResult(
    const ::testing::TestPartResult& test_part_result)
{
    if(test_part_result.failed() && awaiting_check_)
    {
        std::cout
            << std::endl
            << "*** Failure while awaiting call to wraptest_ok() for return value: "
            << std::endl
            << "\t" << return_value_
            << std::endl
            << std::endl;
    }
}

void CheckReturnListener::wait_for_check(std::string return_value)
{
    CheckReturnListener::return_value_ = return_value;
    CheckReturnListener::awaiting_check_ = true;
}
void CheckReturnListener::check_successful()
{
    CheckReturnListener::return_value_   = "";
    CheckReturnListener::awaiting_check_ = false;
}

bool CheckReturnListener::awaiting_check_;
std::string CheckReturnListener::return_value_;
