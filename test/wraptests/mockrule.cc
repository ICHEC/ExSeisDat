#include "mockrule.hh"

namespace exseis {
namespace piol {

::testing::StrictMock<MockRule>& mock_rule()
{
    static ::testing::StrictMock<MockRule> mock_rule;
    return mock_rule;
}

}  // namespace piol
}  // namespace exseis
