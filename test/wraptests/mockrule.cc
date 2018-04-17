#include "mockrule.hh"

namespace exseis {
namespace PIOL {

::testing::StrictMock<MockRule>& mockRule()
{
    static ::testing::StrictMock<MockRule> mockRule;
    return mockRule;
}

}  // namespace PIOL
}  // namespace exseis
