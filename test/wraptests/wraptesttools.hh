#ifndef PIOLWRAPTESTSWRAPTESTTOOLS_HEADER_GUARD
#define PIOLWRAPTESTSWRAPTESTTOOLS_HEADER_GUARD


#include "gmock/gmock.h"
#include "checkreturnlistener.hh"


namespace PIOL {

// A global instance of CheckReturnListener.
// This should be set to the pointer passed into the gtest listeners.
PIOL::CheckReturnListener* checkReturnListener = nullptr;

// Returns a mocked function which, when called, will call
// checkReturnListener->got_expected_return_value();
testing::MockFunction<void()>& returnChecker();

}


// EqDeref(p): *p == arg
MATCHER_P(EqDeref, p, "")
{
    return *p == arg;
}

// AddressEqDeref(p): *p == &arg
MATCHER_P(AddressEqDeref, p, "")
{
    return *p == &arg;
}

ACTION_P(CheckReturn, v)
{
    PIOL::checkReturnListener->expect_return_value(testing::PrintToString(v));
    return v;
}

ACTION(ClearCheckReturn)
{
    PIOL::checkReturnListener->got_expected_return_value();
}


#endif
