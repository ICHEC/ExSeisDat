#include "wraptesttools.hh"

namespace exseis {
namespace piol {

CheckReturnListener*& check_return_listener()
{
    static piol::CheckReturnListener* check_return_listener = nullptr;
    return check_return_listener;
}

::testing::StrictMock<::testing::MockFunction<void()>>& return_checker()
{
    static ::testing::StrictMock<::testing::MockFunction<void()>>
      return_checker;
    return return_checker;
}

}  // namespace piol
}  // namespace exseis
