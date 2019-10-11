#ifndef EXSEISDAT_TEST_SPECTESTS_DYNSEGYMDTEST_HH
#define EXSEISDAT_TEST_SPECTESTS_DYNSEGYMDTEST_HH

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

#include "exseisdat/piol/metadata/Trace_metadata.hh"
#include "exseisdat/piol/metadata/rules/Segy_rule_entry.hh"

#include <memory>

using namespace testing;
using namespace exseis::piol;

struct RuleFix : public Test {
    std::shared_ptr<Rule> rule;
};

struct RuleFixList : public RuleFix {
    std::vector<Trace_metadata_key> meta;
    std::vector<Trace_header_offsets> locs;
    RuleFixList()
    {
        locs = {Trace_header_offsets::x_src, Trace_header_offsets::y_src,
                Trace_header_offsets::x_rcv, Trace_header_offsets::y_rcv};
        meta = {Trace_metadata_key::x_src, Trace_metadata_key::y_src,
                Trace_metadata_key::x_rcv, Trace_metadata_key::y_rcv};
        rule = std::make_shared<Rule>(
            std::initializer_list<Trace_metadata_key>{
                Trace_metadata_key::x_src, Trace_metadata_key::y_src,
                Trace_metadata_key::x_rcv, Trace_metadata_key::y_rcv},
            false);
        rule->rm_rule(Trace_metadata_key::ltn);
        rule->rm_rule(Trace_metadata_key::gtn);
    }
};

struct RuleFixEmpty : public RuleFix {
    RuleFixEmpty() { rule = std::make_shared<Rule>(false, false); }
};


struct RuleFixDefault : public RuleFix {
    RuleFixDefault() { rule = std::make_shared<Rule>(true, true); }
};

#endif  // EXSEISDAT_TEST_SPECTESTS_DYNSEGYMDTEST_HH
