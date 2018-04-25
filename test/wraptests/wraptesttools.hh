#ifndef PIOLWRAPTESTSWRAPTESTTOOLS_HEADER_GUARD
#define PIOLWRAPTESTSWRAPTESTTOOLS_HEADER_GUARD

#include "checkreturnlistener.hh"
#include "printers.hh"

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"


namespace exseis {
namespace PIOL {

// A global instance of CheckReturnListener.
// This should be set to the pointer passed into the gtest listeners.
CheckReturnListener*& checkReturnListener();

// Returns a mocked function which, when called, will call
// checkReturnListener->got_expected_return_value();
::testing::StrictMock<::testing::MockFunction<void()>>& returnChecker();

}  // namespace PIOL
}  // namespace exseis


// EqDeref(p): *p == arg
MATCHER_P(EqDeref, p, "EqDeref")
{
    return *p == arg;
}

// GetEqDeref(p) *p == arg.get()
MATCHER_P(GetEqDeref, p, "GetEqDeref")
{
    return *p == arg.get();
}

// AddressEqDeref(p): *p == &arg
MATCHER_P(AddressEqDeref, p, "AddressEqDeref")
{
    return *p == &arg;
}


ACTION_P(CheckReturn, v)
{
    exseis::PIOL::checkReturnListener()->expect_return_value(
      testing::PrintToString(v));
    return v;
}

ACTION_P(CheckInOutParam, v)
{
    exseis::PIOL::checkReturnListener()->expect_return_value(
      testing::PrintToString(v));
}

ACTION(ClearCheckReturn)
{
    exseis::PIOL::checkReturnListener()->got_expected_return_value();
}


#endif
