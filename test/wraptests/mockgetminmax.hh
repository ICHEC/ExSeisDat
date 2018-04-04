#ifndef PIOLWRAPTESTSMOCKMINMAX_HEADER_GUARD
#define PIOLWRAPTESTSMOCKMINMAX_HEADER_GUARD

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

#include "ExSeisDat/PIOL/operations/minmax.hh"

namespace PIOL {

class MockGetMinMax;
::testing::StrictMock<MockGetMinMax>& mockGetMinMax();

class MockGetMinMax {
  public:
    MOCK_METHOD7(
      getMinMax,
      void(
        ExSeisPIOL* piol,
        size_t offset,
        size_t sz,
        Meta m1,
        Meta m2,
        const Param* prm,
        CoordElem* minmax));
};

}  // namespace PIOL

#endif  // PIOLWRAPTESTSMOCKMINMAX_HEADER_GUARD
