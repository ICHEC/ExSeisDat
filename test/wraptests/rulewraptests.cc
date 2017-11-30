#include "wraptesttools.hh"
#include "rulewraptests.hh"
#include "mockrule.hh"

using namespace PIOL;
using namespace PIOL::File;
using namespace testing;


void test_PIOL_File_Rule()
{
    auto& mockRule = MockRule::instance();

    auto rule_ptr = std::make_shared<Rule*>();
    EXPECT_CALL(mockRule, ctor(_, true, true, false))
        .WillOnce(SaveArg<0>(rule_ptr));

    auto rule_tmp_ptr = std::make_shared<const Rule*>();
    EXPECT_CALL(mockRule, ctor(_, true, false, false))
        .WillOnce(SaveArg<0>(rule_tmp_ptr));

    std::vector<Meta> test_meta_values = {
        PIOL_META_COPY,
        PIOL_META_ltn,
        PIOL_META_gtn,
        PIOL_META_tnl,
        PIOL_META_tnr,
        PIOL_META_tn,
        PIOL_META_tne,
        PIOL_META_ns,
        PIOL_META_inc,
        PIOL_META_Tic,
        PIOL_META_SrcNum,
        PIOL_META_ShotNum,
        PIOL_META_VStack,
        PIOL_META_HStack,
        PIOL_META_Offset,
        PIOL_META_RGElev,
        PIOL_META_SSElev,
        PIOL_META_SDElev,
        PIOL_META_WtrDepSrc,
        PIOL_META_WtrDepRcv,
        PIOL_META_xSrc,
        PIOL_META_ySrc,
        PIOL_META_xRcv,
        PIOL_META_yRcv,
        PIOL_META_xCmp,
        PIOL_META_yCmp,
        PIOL_META_il,
        PIOL_META_xl,
        PIOL_META_TransUnit,
        PIOL_META_TraceUnit,
        PIOL_META_dsdr,
        PIOL_META_Misc1,
        PIOL_META_Misc2,
        PIOL_META_Misc3,
        PIOL_META_Misc4,
    };
    auto rule_tmp2_ptr = std::make_shared<Rule*>();
    EXPECT_CALL(mockRule, ctor(_, test_meta_values, true, false, false))
        .WillOnce(SaveArg<0>(rule_tmp2_ptr));

    EXPECT_CALL(mockRule, addRule(EqDeref(rule_ptr), PIOL_META_COPY)).WillOnce(CheckReturn(true));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());
    EXPECT_CALL(mockRule, addRule(EqDeref(rule_ptr), PIOL_META_COPY)).WillOnce(CheckReturn(false));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mockRule, addRule(EqDeref(rule_ptr), Matcher<const Rule&>(AddressEqDeref(rule_tmp_ptr))));

    EXPECT_CALL(mockRule, addLong(_, PIOL_META_COPY, PIOL_TR_SeqNum));
    EXPECT_CALL(mockRule, addSEGYFloat(_, PIOL_META_COPY, PIOL_TR_SeqNum, PIOL_TR_SeqNum));
    EXPECT_CALL(mockRule, addShort(_, PIOL_META_COPY, PIOL_TR_SeqNum));
    EXPECT_CALL(mockRule, addIndex(_, PIOL_META_COPY));
    EXPECT_CALL(mockRule, addCopy(_));

    EXPECT_CALL(mockRule, rmRule(_, PIOL_META_COPY));

    EXPECT_CALL(mockRule, extent(_)).WillOnce(CheckReturn(100));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mockRule, memUsage(_)).WillOnce(CheckReturn(110));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mockRule, paramMem(_)).WillOnce(CheckReturn(120));
    EXPECT_CALL(returnChecker(), Call()).WillOnce(ClearCheckReturn());

    //MOCK_METHOD1(getEntry, RuleEntry * (Meta entry));

    EXPECT_CALL(mockRule, dtor(EqDeref(rule_tmp2_ptr)));
    EXPECT_CALL(mockRule, dtor(EqDeref(rule_tmp_ptr)));
}
