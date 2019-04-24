#ifndef EXSEISDAT_TEST_SPECTESTS_DYNSEGYMDTEST_HH
#define EXSEISDAT_TEST_SPECTESTS_DYNSEGYMDTEST_HH

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "tglobal.hh"

#include "exseisdat/piol/SEGYRuleEntry.hh"
#include "exseisdat/piol/Trace_metadata.hh"

#include <memory>

using namespace testing;
using namespace exseis::piol;

struct RuleFix : public Test {
    std::shared_ptr<Rule> rule;
};

struct RuleFixList : public RuleFix {
    std::vector<Meta> meta;
    std::vector<Tr> locs;
    RuleFixList()
    {
        locs = {Tr::x_src, Tr::y_src, Tr::x_rcv, Tr::y_rcv};
        meta = {Meta::x_src, Meta::y_src, Meta::x_rcv, Meta::y_rcv};
        rule = std::make_shared<Rule>(
            std::initializer_list<Meta>{Meta::x_src, Meta::y_src, Meta::x_rcv,
                                        Meta::y_rcv},
            false);
        rule->rm_rule(Meta::ltn);
        rule->rm_rule(Meta::gtn);
    }
};

struct RuleFixEmpty : public RuleFix {
    RuleFixEmpty() { rule = std::make_shared<Rule>(false, false); }
};


struct RuleFixDefault : public RuleFix {
    RuleFixDefault() { rule = std::make_shared<Rule>(true, true); }
};

#endif  // EXSEISDAT_TEST_SPECTESTS_DYNSEGYMDTEST_HH
