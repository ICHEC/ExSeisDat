#include "mockgetminmax.hh"

namespace exseis {
namespace PIOL {

::testing::StrictMock<MockGetMinMax>& mockGetMinMax()
{
    static ::testing::StrictMock<MockGetMinMax> mockGetMinMax;
    return mockGetMinMax;
}

}  // namespace PIOL
}  // namespace exseis
