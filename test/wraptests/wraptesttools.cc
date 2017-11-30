#include "wraptesttools.hh"

namespace PIOL {

testing::MockFunction<void()>& returnChecker()
{
    static testing::MockFunction<void()> returnChecker;
    return returnChecker;
}

} // namespace PIOL
