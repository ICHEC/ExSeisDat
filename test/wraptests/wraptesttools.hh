#ifndef PIOLWRAPTESTSWRAPTESTTOOLS_HEADER_GUARD
#define PIOLWRAPTESTSWRAPTESTTOOLS_HEADER_GUARD


#include "printers.hh"
#include "gmock/gmock.h"
#include "checkreturnlistener.hh"

#include "cppfileapi.hh"


namespace PIOL {

// A global instance of CheckReturnListener.
// This should be set to the pointer passed into the gtest listeners.
PIOL::CheckReturnListener*& checkReturnListener();

// Returns a mocked function which, when called, will call
// checkReturnListener->got_expected_return_value();
::testing::StrictMock<::testing::MockFunction<void()>>& returnChecker();

}


// EqDeref(p): *p == arg
MATCHER_P(EqDeref, p, "")
{
    return *p == arg;
}

// GetEqDeref(p) *p == arg.get()
MATCHER_P(GetEqDeref, p, "")
{
    return *p == arg.get();
}

// AddressEqDeref(p): *p == &arg
MATCHER_P(AddressEqDeref, p, "")
{
    return *p == &arg;
}


ACTION_P(CheckReturn, v)
{
    PIOL::checkReturnListener()->expect_return_value(testing::PrintToString(v));
    return v;
}

ACTION_P(CheckInOutParam, v)
{
    PIOL::checkReturnListener()->expect_return_value(testing::PrintToString(v));
}

ACTION(ClearCheckReturn)
{
    PIOL::checkReturnListener()->got_expected_return_value();
}


#endif
