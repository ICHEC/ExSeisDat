#include "wraptesttools.hh"

namespace exseis {
namespace PIOL {

CheckReturnListener*& checkReturnListener()
{
    static PIOL::CheckReturnListener* checkReturnListener = nullptr;
    return checkReturnListener;
}

::testing::StrictMock<::testing::MockFunction<void()>>& returnChecker()
{
    static ::testing::StrictMock<::testing::MockFunction<void()>> returnChecker;
    return returnChecker;
}

}  // namespace PIOL
}  // namespace exseis
