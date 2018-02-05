#ifndef PIOLWRAPTESTSCHECKRETURNLISTENER_HEADER_GUARD
#define PIOLWRAPTESTSCHECKRETURNLISTENER_HEADER_GUARD

#include "gtest/gtest.h"

namespace PIOL {

class CheckReturnListener : public ::testing::EmptyTestEventListener {
  public:
    // Called after EXPECT_CALL throws
    virtual void OnTestPartResult(
      const ::testing::TestPartResult& test_part_result);

    // When expecting the wrapped side to check a return value,
    // call this with a pretty printed string of the return value.
    // If a gtest exception is thrown while expecting a return value
    // (likely from an out-of-order mock call),
    // the listener will print the return_value it was waiting on.
    void expect_return_value(std::string return_value);

    // Call this when the expected return value has been successfully tested.
    void got_expected_return_value();

  private:
    // A flag to say if the listener is awaiting a return value.
    bool expecting_return_value = false;

    // A pretty string representing the expected return value.
    std::string return_value;
};

}  // namespace PIOL

#endif  // PIOLWRAPTESTSCHECKRETURNLISTENER_HEADER_GUARD
