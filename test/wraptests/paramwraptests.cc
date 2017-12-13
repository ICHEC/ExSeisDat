#include "wraptesttools.hh"
#include "paramwraptests.hh"
#include "rulewraptests.hh"
#include "mockparam.hh"

using namespace PIOL;
using namespace PIOL::File;
using namespace testing;


std::shared_ptr<PIOL::File::Param*> test_PIOL_File_Param(
    std::shared_ptr<PIOL::File::Rule*> test_rule
)
{
    auto param_ptr = std::make_shared<Param*>();
    EXPECT_CALL(mockParam(), ctor(_, GetEqDeref(test_rule), 300))
        .WillOnce(SaveArg<0>(param_ptr));

    EXPECT_CALL(mockParam(), ctor(_, 310));

    EXPECT_CALL(
        mockParamFreeFunctions(),
        getPrm_int16_t(320, PIOL_META_COPY, EqDeref(param_ptr))
    ).WillOnce(CheckReturn(330));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
        mockParamFreeFunctions(),
        getPrm_llint(340, PIOL_META_COPY, EqDeref(param_ptr))
    ).WillOnce(CheckReturn(350));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
        mockParamFreeFunctions(),
        getPrm_geom_t(360, PIOL_META_COPY, EqDeref(param_ptr))
    ).WillOnce(CheckReturn(370.0));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
        mockParamFreeFunctions(),
        setPrm_int16_t(380, PIOL_META_COPY, 390, EqDeref(param_ptr))
    );
    EXPECT_CALL(
        mockParamFreeFunctions(),
        setPrm_llint(400, PIOL_META_COPY, 410, EqDeref(param_ptr))
    );
    EXPECT_CALL(
        mockParamFreeFunctions(),
        setPrm_geom_t(
            420, PIOL_META_COPY, DoubleEq(430.0), EqDeref(param_ptr)
        )
    );

    //void PIOL_File_cpyPrm(
    //    size_t i, const PIOL_File_Param* src,
    //    size_t j, PIOL_File_Param* dst
    //);

    //void PIOL_File_Param_delete(PIOL_File_Param* param);

    return param_ptr;
}
