#ifndef PIOLWRAPTESTSMOCKRULE_HEADER_GUARD
#define PIOLWRAPTESTSMOCKRULE_HEADER_GUARD

#include "googletest_variable_instances.hh"
#include "gmock/gmock.h"

#include "ExSeisDat/PIOL/Rule.hh"
#include "ExSeisDat/PIOL/RuleEntry.hh"

namespace exseis {
namespace PIOL {

class MockRule;
::testing::StrictMock<MockRule>& mockRule();

class MockRule {
  public:
    MOCK_METHOD4(ctor, void(Rule*, bool full, bool defaults, bool extras));

    MOCK_METHOD5(
      ctor,
      void(
        Rule*,
        const std::vector<Meta>& m,
        bool full,
        bool defaults,
        bool extras));

    MOCK_METHOD3(ctor, void(Rule*, Rule::RuleMap translate_, bool full));

    MOCK_METHOD1(dtor, void(Rule*));

    MOCK_METHOD2(addRule, bool(Rule*, Meta m));

    MOCK_METHOD2(addRule, bool(Rule*, const Rule& r));

    MOCK_METHOD3(addLong, void(Rule*, Meta m, Tr loc));

    MOCK_METHOD4(addSEGYFloat, void(Rule*, Meta m, Tr loc, Tr scalLoc));

    MOCK_METHOD3(addShort, void(Rule*, Meta m, Tr loc));

    MOCK_METHOD2(addIndex, void(Rule*, Meta m));

    MOCK_METHOD1(addCopy, void(Rule*));

    MOCK_METHOD2(rmRule, void(Rule*, Meta m));

    MOCK_METHOD1(extent, size_t(Rule*));

    MOCK_CONST_METHOD1(memUsage, size_t(const Rule*));

    MOCK_CONST_METHOD1(paramMem, size_t(const Rule*));

    MOCK_METHOD2(getEntry, RuleEntry*(Rule*, Meta entry));
};

}  // namespace PIOL
}  // namespace exseis

#endif  // PIOLWRAPTESTSMOCKRULE_HEADER_GUARD
