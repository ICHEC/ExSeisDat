#include "getminmaxwraptests.hh"
#include "mockgetminmax.hh"
#include "wraptesttools.hh"

using namespace PIOL;
using namespace testing;

void test_PIOL_File_getMinMax(
  std::shared_ptr<PIOL::ExSeis*> piol, std::shared_ptr<PIOL::Param*> param)
{
    EXPECT_CALL(
      mockGetMinMax(), getMinMax(
                         EqDeref(piol), 520, 530, PIOL_META_COPY,
                         PIOL_META_COPY, EqDeref(param), _))
      .WillOnce(DoAll(
        WithArg<6>(Invoke([](auto coordElem) {
            EXPECT_FLOAT_EQ(coordElem->val, 500.0);
            EXPECT_EQ(coordElem->num, 510u);

            coordElem->val = 540.0;
            coordElem->num = 550.0;
        })),
        CheckInOutParam(CoordElem{540.0, 550})));

    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());
}
