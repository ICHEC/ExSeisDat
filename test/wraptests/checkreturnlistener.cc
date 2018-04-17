#include "checkreturnlistener.hh"

namespace exseis {
namespace PIOL {

void CheckReturnListener::OnTestPartResult(
  const ::testing::TestPartResult& test_part_result)
{
    if (test_part_result.failed() && expecting_return_value) {
        std::cout
          << std::endl
          << "*** Failure while awaiting call to wraptest_ok() for return value: "
          << std::endl
          << "\t" << return_value << std::endl
          << std::endl;
    }
}

void CheckReturnListener::expect_return_value(std::string return_value)
{
    this->return_value     = return_value;
    expecting_return_value = true;
}

void CheckReturnListener::got_expected_return_value()
{
    return_value.clear();
    expecting_return_value = false;
}

}  // namespace PIOL
}  // namespace exseis
