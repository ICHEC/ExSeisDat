#include <memory>
#include "file/dynsegymd.hh"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;
using namespace PIOL;
using namespace File;

struct RuleFix : public Test
{
    std::shared_ptr<Rule> rule;
    RuleFix(void)
    {
        rule = NULL;
    }
    ~RuleFix(void)
    {
        if (rule)
            delete rule;
    }
};

struct RuleFixList : public RuleFix
{
    std::vector<Meta> meta;
    std::vector<size_t> locs = {size_t(Tr::xSrc), size_t(Tr::ySrc), size_t(Tr::xRcv), size_t(Tr::yRcv)};
    RuleFixList(void) : meta(std::vector<Meta>{Meta::xSrc, Meta::ySrc, Meta::xRcv, Meta::yRcv}),
                        locs(std::vector<size_t>{size_t(Tr::xSrc), size_t(Tr::ySrc), size_t(Tr::xRcv), size_t(Tr::yRcv)})
    {
        rule = std::make_shared<Rule>(false, meta);
    }
};

struct RuleFixEmpty : public RuleFix
{
    RuleFixEmpty(void)
    {
        rule = std::make_shared<Rule>(false, false);
    }
};


struct RuleFixDefault : public RuleFix
{
    RuleFixDefault(void)
    {
        rule = std::make_shared<Rule>(true, true);
    }
};
