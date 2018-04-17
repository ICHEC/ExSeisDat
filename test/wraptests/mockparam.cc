#include "mockparam.hh"

namespace exseis {
namespace PIOL {

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

}  // namespace PIOL
}  // namespace exseis
