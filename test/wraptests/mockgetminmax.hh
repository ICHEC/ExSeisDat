#ifndef EXSEISDAT_TEST_WRAPTESTS_MOCKGETMINMAX_HH
#define EXSEISDAT_TEST_WRAPTESTS_MOCKGETMINMAX_HH

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

#include "exseisdat/piol/operations/minmax.hh"

namespace exseis {
namespace piol {

class MockGetMinMax;
::testing::StrictMock<MockGetMinMax>& mock_get_min_max();

class MockGetMinMax {
  public:
    MOCK_METHOD7(
      get_min_max,
      void(
        ExSeisPIOL* piol,
        size_t offset,
        size_t sz,
        Meta m1,
        Meta m2,
        const Trace_metadata& prm,
        CoordElem* minmax));
};

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_TEST_WRAPTESTS_MOCKGETMINMAX_HH
