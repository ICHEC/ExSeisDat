#include "checkreturnlistener.hh"

namespace exseis {
namespace piol {

void CheckReturnListener::OnTestPartResult(
    const ::testing::TestPartResult& test_part_result)
{
    if (test_part_result.failed() && m_expecting_return_value) {
        std::cout
            << std::endl
            << "*** Failure while awaiting call to wraptest_ok() for return value: "
            << std::endl
            << "\t" << m_return_value << std::endl
            << std::endl;
    }
}

void CheckReturnListener::expect_return_value(std::string return_value)
{
    m_return_value           = return_value;
    m_expecting_return_value = true;
}

void CheckReturnListener::got_expected_return_value()
{
    m_return_value.clear();
    m_expecting_return_value = false;
}

}  // namespace piol
}  // namespace exseis
