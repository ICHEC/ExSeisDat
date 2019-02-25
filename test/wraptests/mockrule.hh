#ifndef EXSEISDAT_TEST_WRAPTESTS_MOCKRULE_HH
#define EXSEISDAT_TEST_WRAPTESTS_MOCKRULE_HH

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

#include "exseisdat/piol/Rule.hh"
#include "exseisdat/piol/RuleEntry.hh"

namespace exseis {
namespace piol {

class MockRule;
::testing::StrictMock<MockRule>& mock_rule();

class MockRule {
  public:
    MOCK_METHOD4(Rule_ctor, void(Rule*, bool full, bool defaults, bool extras));

    MOCK_METHOD5(
        Rule_ctor,
        void(
            Rule*,
            const std::vector<Meta>& m,
            bool full,
            bool defaults,
            bool extras));

    MOCK_METHOD3(
        Rule_ctor,
        void(Rule*, const Rule::Rule_entry_map& rule_entry_map_, bool full));

    MOCK_METHOD1(Rule_dtor, void(Rule*));

    MOCK_METHOD2(add_rule, bool(Rule*, Meta m));

    MOCK_METHOD2(add_rule, bool(Rule*, const Rule& r));

    MOCK_METHOD3(add_long, void(Rule*, Meta m, Tr loc));

    MOCK_METHOD4(
        add_segy_float, void(Rule*, Meta m, Tr loc, Tr scalar_location));

    MOCK_METHOD3(add_short, void(Rule*, Meta m, Tr loc));

    MOCK_METHOD2(add_index, void(Rule*, Meta m));

    MOCK_METHOD1(add_copy, void(Rule*));

    MOCK_METHOD2(rm_rule, void(Rule*, Meta m));

    MOCK_METHOD1(extent, size_t(Rule*));

    MOCK_CONST_METHOD1(memory_usage, size_t(const Rule*));

    MOCK_CONST_METHOD1(memory_usage_per_header, size_t(const Rule*));

    MOCK_CONST_METHOD2(get_entry, const RuleEntry*(const Rule*, Meta entry));
};

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_TEST_WRAPTESTS_MOCKRULE_HH
