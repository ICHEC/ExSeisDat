#include "mockrule.hh"
#include "rulewraptests.hh"
#include "wraptesttools.hh"

using namespace testing;
using namespace exseis::piol;


std::shared_ptr<Rule*> test_piol_file_rule()
{
    auto rule_ptr = std::make_shared<Rule*>();
    EXPECT_CALL(mock_rule(), Rule_ctor(_, true, true, false))
      .WillOnce(SaveArg<0>(rule_ptr));

    auto rule_tmp_ptr = std::make_shared<const Rule*>();
    EXPECT_CALL(mock_rule(), Rule_ctor(_, true, false, false))
      .WillOnce(SaveArg<0>(rule_tmp_ptr));

    const std::vector<Meta> metas = {
      Meta::Copy,      Meta::ltn,       Meta::gtn,
      Meta::tnl,       Meta::tnr,       Meta::tn,
      Meta::tne,       Meta::ns,        Meta::sample_interval,
      Meta::Tic,       Meta::SrcNum,    Meta::ShotNum,
      Meta::VStack,    Meta::HStack,    Meta::Offset,
      Meta::RGElev,    Meta::SSElev,    Meta::SDElev,
      Meta::WtrDepSrc, Meta::WtrDepRcv, Meta::x_src,
      Meta::y_src,     Meta::x_rcv,     Meta::y_rcv,
      Meta::xCmp,      Meta::yCmp,      Meta::il,
      Meta::xl,        Meta::TransUnit, Meta::TraceUnit,
      Meta::dsdr,      Meta::Misc1,     Meta::Misc2,
      Meta::Misc3,     Meta::Misc4,
    };

    auto rule_tmp2_ptr = std::make_shared<Rule*>();
    EXPECT_CALL(mock_rule(), Rule_ctor(_, metas, true, false, false))
      .WillOnce(SaveArg<0>(rule_tmp2_ptr));

    EXPECT_CALL(mock_rule(), add_rule(EqDeref(rule_ptr), Meta::Copy))
      .WillOnce(CheckReturn(true));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mock_rule(), add_rule(EqDeref(rule_ptr), Meta::Copy))
      .WillOnce(CheckReturn(false));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
      mock_rule(),
      add_rule(
        EqDeref(rule_ptr), Matcher<const Rule&>(AddressEqDeref(rule_tmp_ptr))));

    EXPECT_CALL(mock_rule(), add_long(_, Meta::Copy, Tr::SeqNum));
    EXPECT_CALL(
      mock_rule(), add_segy_float(_, Meta::Copy, Tr::SeqNum, Tr::SeqNum));
    EXPECT_CALL(mock_rule(), add_short(_, Meta::Copy, Tr::SeqNum));
    EXPECT_CALL(mock_rule(), add_index(_, Meta::Copy));
    EXPECT_CALL(mock_rule(), add_copy(_));

    EXPECT_CALL(mock_rule(), rm_rule(_, Meta::Copy));

    EXPECT_CALL(mock_rule(), extent(_)).WillOnce(CheckReturn(100));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mock_rule(), memory_usage(_)).WillOnce(CheckReturn(110));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mock_rule(), memory_usage_per_header(_))
      .WillOnce(CheckReturn(120));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    // MOCK_METHOD1(get_entry, RuleEntry * (Meta entry));

    EXPECT_CALL(mock_rule(), Rule_dtor(EqDeref(rule_tmp2_ptr)));
    EXPECT_CALL(mock_rule(), Rule_dtor(EqDeref(rule_tmp_ptr)));

    return rule_ptr;
}
