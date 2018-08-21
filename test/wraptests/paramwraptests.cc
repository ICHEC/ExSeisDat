#include "mockparam.hh"
#include "paramwraptests.hh"
#include "rulewraptests.hh"
#include "wraptesttools.hh"

using namespace testing;
using namespace exseis::PIOL;


std::shared_ptr<Param*> test_PIOL_File_Param(std::shared_ptr<Rule*> test_rule)
{
    auto param_ptr = std::make_shared<Param*>();
    EXPECT_CALL(mockParam(), ctor(_, GetEqDeref(test_rule), 300))
      .WillOnce(SaveArg<0>(param_ptr));

    auto param_ptr_2 = std::make_shared<Param*>();
    EXPECT_CALL(mockParam(), ctor(_, 310)).WillOnce(SaveArg<0>(param_ptr_2));

    EXPECT_CALL(mockParam(), size(EqDeref(param_ptr)))
      .WillOnce(CheckReturn(320));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());
    EXPECT_CALL(mockParam(), memUsage(EqDeref(param_ptr)))
      .WillOnce(CheckReturn(330));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
      mockParamFreeFunctions(),
      getPrm_int16_t(340, PIOL_META_COPY, EqDeref(param_ptr)))
      .WillOnce(CheckReturn(350));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
      mockParamFreeFunctions(),
      getPrm_Integer(360, PIOL_META_COPY, EqDeref(param_ptr)))
      .WillOnce(CheckReturn(370));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
      mockParamFreeFunctions(),
      getPrm_Floating_point(380, PIOL_META_COPY, EqDeref(param_ptr)))
      .WillOnce(CheckReturn(390.0));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
      mockParamFreeFunctions(),
      setPrm_int16_t(400, PIOL_META_COPY, 410, EqDeref(param_ptr)));
    EXPECT_CALL(
      mockParamFreeFunctions(),
      setPrm_Integer(420, PIOL_META_COPY, 430, EqDeref(param_ptr)));
    EXPECT_CALL(
      mockParamFreeFunctions(),
      setPrm_Floating_point(
        440, PIOL_META_COPY, DoubleEq(450.0), EqDeref(param_ptr)));

    EXPECT_CALL(
      mockParamFreeFunctions(),
      cpyPrm(460, EqDeref(param_ptr), 470, EqDeref(param_ptr_2)));

    EXPECT_CALL(mockParam(), dtor(EqDeref(param_ptr_2)));

    return param_ptr;
}
