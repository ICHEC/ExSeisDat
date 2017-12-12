#include "mockparam.hh"

namespace PIOL {
namespace File {

::testing::StrictMock<MockParam>& mockParam()
{
    static ::testing::StrictMock<MockParam> mockParam;
    return mockParam;
}

::testing::StrictMock<MockParamFreeFunctions>& mockParamFreeFunctions()
{
    static ::testing::StrictMock<MockParamFreeFunctions> mockParamFreeFunctions;
    return mockParamFreeFunctions;
}

} // namespace File
} // namespace PIOL
