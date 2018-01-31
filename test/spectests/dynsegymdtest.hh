#include "file/dynsegymd.hh"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <memory>

using namespace testing;
using namespace PIOL;
using namespace File;

struct RuleFix : public Test {
    std::shared_ptr<Rule> rule;
    RuleFix(void) { rule = NULL; }
    ~RuleFix(void) {}
};

struct RuleFixList : public RuleFix {
    std::vector<Meta> meta;
    std::vector<size_t> locs;
    RuleFixList(void)
    {
        locs = {PIOL_TR_xSrc, PIOL_TR_ySrc, PIOL_TR_xRcv, PIOL_TR_yRcv};
        meta = {PIOL_META_xSrc, PIOL_META_ySrc, PIOL_META_xRcv, PIOL_META_yRcv};
        rule = std::make_shared<Rule>(
          std::initializer_list<Meta>{PIOL_META_xSrc, PIOL_META_ySrc,
                                      PIOL_META_xRcv, PIOL_META_yRcv},
          false);
        rule->rmRule(PIOL_META_ltn);
        rule->rmRule(PIOL_META_gtn);
    }
};

struct RuleFixEmpty : public RuleFix {
    RuleFixEmpty(void) { rule = std::make_shared<Rule>(false, false); }
};


struct RuleFixDefault : public RuleFix {
    RuleFixDefault(void) { rule = std::make_shared<Rule>(true, true); }
};
