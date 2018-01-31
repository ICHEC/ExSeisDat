#include "mockrule.hh"

namespace PIOL {
namespace File {

::testing::StrictMock<MockRule>& mockRule()
{
    static ::testing::StrictMock<MockRule> mockRule;
    return mockRule;
}

}  // namespace File
}  // namespace PIOL
