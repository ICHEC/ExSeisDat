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
    //void PIOL_File_Param_delete(PIOL_File_Param* param);
    //int16_t PIOL_File_getPrm_short(
    //    size_t i, PIOL_Meta entry, PIOL_File_Param* param
    //);
    //int64_t PIOL_File_getPrm_llint(
    //    size_t i, PIOL_Meta entry, PIOL_File_Param* param
    //);
    //double PIOL_File_getPrm_double(
    //    size_t i, PIOL_Meta entry, PIOL_File_Param* param
    //);
    //void PIOL_File_setPrm_short(
    //    size_t i, PIOL_Meta entry, int16_t ret, PIOL_File_Param* param
    //);
    //void PIOL_File_setPrm_llint(
    //    size_t i, PIOL_Meta entry, int64_t ret, PIOL_File_Param* param
    //);
    //void PIOL_File_setPrm_double(
    //    size_t i, PIOL_Meta entry, double ret, PIOL_File_Param* param
    //);

    //void PIOL_File_cpyPrm(
    //    size_t i, const PIOL_File_Param* src,
    //    size_t j, PIOL_File_Param* dst
    //);

    return param_ptr;
}
