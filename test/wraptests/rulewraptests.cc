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

    const std::vector<Trace_metadata_key> metas = {
        Trace_metadata_key::Copy,
        Trace_metadata_key::ltn,
        Trace_metadata_key::gtn,
        Trace_metadata_key::tnl,
        Trace_metadata_key::tnr,
        Trace_metadata_key::tn,
        Trace_metadata_key::tne,
        Trace_metadata_key::ns,
        Trace_metadata_key::sample_interval,
        Trace_metadata_key::Tic,
        Trace_metadata_key::SrcNum,
        Trace_metadata_key::ShotNum,
        Trace_metadata_key::VStack,
        Trace_metadata_key::HStack,
        Trace_metadata_key::Offset,
        Trace_metadata_key::RGElev,
        Trace_metadata_key::SSElev,
        Trace_metadata_key::SDElev,
        Trace_metadata_key::WtrDepSrc,
        Trace_metadata_key::WtrDepRcv,
        Trace_metadata_key::x_src,
        Trace_metadata_key::y_src,
        Trace_metadata_key::x_rcv,
        Trace_metadata_key::y_rcv,
        Trace_metadata_key::xCmp,
        Trace_metadata_key::yCmp,
        Trace_metadata_key::il,
        Trace_metadata_key::xl,
        Trace_metadata_key::TransUnit,
        Trace_metadata_key::TraceUnit,
        Trace_metadata_key::dsdr,
        Trace_metadata_key::Misc1,
        Trace_metadata_key::Misc2,
        Trace_metadata_key::Misc3,
        Trace_metadata_key::Misc4,
    };

    auto rule_tmp2_ptr = std::make_shared<Rule*>();
    EXPECT_CALL(mock_rule(), Rule_ctor(_, metas, true, false, false))
        .WillOnce(SaveArg<0>(rule_tmp2_ptr));

    EXPECT_CALL(
        mock_rule(), add_rule(EqDeref(rule_ptr), Trace_metadata_key::Copy))
        .WillOnce(CheckReturn(true));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
        mock_rule(), add_rule(EqDeref(rule_ptr), Trace_metadata_key::Copy))
        .WillOnce(CheckReturn(false));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(
        mock_rule(), add_rule(
                         EqDeref(rule_ptr),
                         Matcher<const Rule&>(AddressEqDeref(rule_tmp_ptr))));

    EXPECT_CALL(
        mock_rule(),
        add_long(
            _, Trace_metadata_key::Copy, segy::Trace_header_offsets::SeqNum));
    EXPECT_CALL(
        mock_rule(),
        add_segy_float(
            _, Trace_metadata_key::Copy, segy::Trace_header_offsets::SeqNum,
            segy::Trace_header_offsets::SeqNum));
    EXPECT_CALL(
        mock_rule(),
        add_short(
            _, Trace_metadata_key::Copy, segy::Trace_header_offsets::SeqNum));
    EXPECT_CALL(mock_rule(), add_index(_, Trace_metadata_key::Copy));
    EXPECT_CALL(mock_rule(), add_copy(_));

    EXPECT_CALL(mock_rule(), rm_rule(_, Trace_metadata_key::Copy));

    EXPECT_CALL(mock_rule(), extent(_)).WillOnce(CheckReturn(100));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mock_rule(), memory_usage(_)).WillOnce(CheckReturn(110));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    EXPECT_CALL(mock_rule(), memory_usage_per_header(_))
        .WillOnce(CheckReturn(120));
    EXPECT_CALL(return_checker(), Call()).WillOnce(ClearCheckReturn());

    // MOCK_METHOD1(get_entry, Rule_entry * (Trace_metadata_key entry));

    EXPECT_CALL(mock_rule(), Rule_dtor(EqDeref(rule_tmp2_ptr)));
    EXPECT_CALL(mock_rule(), Rule_dtor(EqDeref(rule_tmp_ptr)));

    return rule_ptr;
}
