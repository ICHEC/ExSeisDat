#include "wraptesttools.hh"

namespace PIOL {

PIOL::CheckReturnListener*& checkReturnListener()
{
    static PIOL::CheckReturnListener* checkReturnListener = nullptr;
    return checkReturnListener;
}

testing::MockFunction<void()>& returnChecker()
{
    static testing::MockFunction<void()> returnChecker;
    return returnChecker;
}

} // namespace PIOL
