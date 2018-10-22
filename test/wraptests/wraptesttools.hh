#ifndef EXSEISDAT_TEST_WRAPTESTS_WRAPTESTTOOLS_HH
#define EXSEISDAT_TEST_WRAPTESTS_WRAPTESTTOOLS_HH

#include "checkreturnlistener.hh"
#include "printers.hh"

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"


namespace exseis {
namespace piol {

// A global instance of CheckReturnListener.
// This should be set to the pointer passed into the gtest listeners.
CheckReturnListener*& check_return_listener();

// Returns a mocked function which, when called, will call
// check_return_listener->got_expected_return_value();
::testing::StrictMock<::testing::MockFunction<void()>>& return_checker();

}  // namespace piol
}  // namespace exseis


// EqDeref(p): *p == arg
MATCHER_P(EqDeref, p, "EqDeref")
{
    return *p == arg;
}

// EqDerefDeref(p): **p == arg
MATCHER_P(EqDerefDeref, p, "EqDerefDeref")
{
    return **p == arg;
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
    exseis::piol::check_return_listener()->expect_return_value(
      testing::PrintToString(v));
    return v;
}

ACTION_P(CheckInOutParam, v)
{
    exseis::piol::check_return_listener()->expect_return_value(
      testing::PrintToString(v));
}

ACTION(ClearCheckReturn)
{
    exseis::piol::check_return_listener()->got_expected_return_value();
}


#endif  // EXSEISDAT_TEST_WRAPTESTS_WRAPTESTTOOLS_HH
