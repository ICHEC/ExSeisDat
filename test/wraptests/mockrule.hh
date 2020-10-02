#ifndef EXSEISDAT_TEST_WRAPTESTS_MOCKRULE_HH
#define EXSEISDAT_TEST_WRAPTESTS_MOCKRULE_HH

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

#include "exseisdat/piol/metadata/rules/Rule.hh"
#include "exseisdat/piol/metadata/rules/Rule_entry.hh"

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
            const std::vector<Trace_metadata_key>& m,
            bool full,
            bool defaults,
            bool extras));

    MOCK_METHOD3(
        Rule_ctor,
        void(Rule*, const Rule::Rule_entry_map& rule_entry_map_, bool full));

    MOCK_METHOD1(Rule_dtor, void(Rule*));

    MOCK_METHOD2(add_rule, bool(Rule*, Trace_metadata_key m));

    MOCK_METHOD2(add_rule, bool(Rule*, const Rule& r));

    MOCK_METHOD3(
        add_long,
        void(Rule*, Trace_metadata_key m, segy::Trace_header_offsets loc));

    MOCK_METHOD4(
        add_segy_float,
        void(
            Rule*,
            Trace_metadata_key m,
            segy::Trace_header_offsets loc,
            segy::Trace_header_offsets scalar_location));

    MOCK_METHOD3(
        add_short,
        void(Rule*, Trace_metadata_key m, segy::Trace_header_offsets loc));

    MOCK_METHOD2(add_index, void(Rule*, Trace_metadata_key m));

    MOCK_METHOD1(add_copy, void(Rule*));

    MOCK_METHOD2(rm_rule, void(Rule*, Trace_metadata_key m));

    MOCK_METHOD1(extent, size_t(Rule*));

    MOCK_CONST_METHOD1(memory_usage, size_t(const Rule*));

    MOCK_CONST_METHOD1(memory_usage_per_header, size_t(const Rule*));

    MOCK_CONST_METHOD2(
        get_entry, const Rule_entry*(const Rule*, Trace_metadata_key entry));
};

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_TEST_WRAPTESTS_MOCKRULE_HH
