#include "getminmaxwraptests.hh"
#include "mockgetminmax.hh"
#include "wraptesttools.hh"

using namespace testing;
using namespace exseis::piol;

void test_piol_file_get_min_max(
  std::shared_ptr<ExSeis*> piol, std::shared_ptr<Trace_metadata*> param)
{
    EXPECT_CALL(
      mock_get_min_max(), get_min_max(
                            EqDeref(piol), 520, 530, Meta::Copy, Meta::Copy,
                            AddressEqDeref(param), _))
      .WillOnce(DoAll(
        WithArg<6>(Invoke([](auto coord_elem) {
            EXPECT_FLOAT_EQ(coord_elem->val, 500.0);
            EXPECT_EQ(coord_elem->num, 510u);

            coord_elem->val = 540.0;
            coord_elem->num = 550.0;
        })),
        CheckInOutParam(CoordElem{540.0, 550})));

    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());
}
